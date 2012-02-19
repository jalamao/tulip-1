/**
 *
 * This file is part of Tulip (www.tulip-software.org)
 *
 * Authors: David Auber and the Tulip development Team
 * from LaBRI, University of Bordeaux 1 and Inria Bordeaux - Sud Ouest
 *
 * Tulip is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation, either version 3
 * of the License, or (at your option) any later version.
 *
 * Tulip is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 */

#include <QtCore/QTextStream>
#include <QtCore/QRegExp>
#include <QtCore/QStringList>
#include <QtCore/QString>

#include <iostream>

#include <tulip/TemplateFactory.h>
#include <tulip/Algorithm.h>
#include <tulip/ForEach.h>
#include <tulip/StringCollection.h>

#include "AutoCompletionDataBase.h"
#include "PythonInterpreter.h"

using namespace std;
using namespace tlp;

AutoCompletionDataBase::AutoCompletionDataBase(APIDataBase *apiDb) : graph(NULL), apiDb(apiDb) {
    iteratorType["tlp.IteratorNode"] = "tlp.node";
    iteratorType["tlp.NodeMapIterator"] = "tlp.node";
    iteratorType["tlp.IteratorEdge"] = "tlp.edge";
    iteratorType["tlp.EdgeMapIterator"] = "tlp.edge";
    iteratorType["tlp.IteratorGraph"] = "tlp.Graph";
    iteratorType["tlp.IteratorString"] = "string";
}

static bool tlpPluginExists(const QString &pluginName) {
    tlp::TemplateFactoryInterface *selectedFactory = NULL;
    std::map< std::string, tlp::TemplateFactoryInterface* >::iterator it = tlp::TemplateFactoryInterface::allFactories->begin();

    for (; it != tlp::TemplateFactoryInterface::allFactories->end() ; ++it) {
        if (it->second->pluginExists(pluginName.toStdString())) {
            selectedFactory = it->second;
            break;
        }
    }

    return selectedFactory != NULL;
}

static QString getPythonTypeName(const QString &cppTypeName) {
    if (cppTypeName == "b") {
        return "boolean";
    }
    else if (cppTypeName == "i") {
        return "integer";
    }
    else if (cppTypeName == "d") {
        return "float";
    }
    else {
        QString typeName = tlp::demangleTlpClassName(cppTypeName.toStdString().c_str()).c_str();
        typeName.replace("*", "");
        return "tlp." + typeName;
    }
}

static QSet<QString> getParametersListForPlugin(const QString &pluginName, const QString &prefix="") {
    QSet<QString> ret;
    tlp::TemplateFactoryInterface *selectedFactory = NULL;
    std::map< std::string, tlp::TemplateFactoryInterface* >::iterator it = tlp::TemplateFactoryInterface::allFactories->begin();

    for (; it != tlp::TemplateFactoryInterface::allFactories->end() ; ++it) {
        if (it->second->pluginExists(pluginName.toStdString())) {
            selectedFactory = it->second;
            break;
        }
    }

    if (selectedFactory) {
        const tlp::ParameterDescriptionList&  parameters = selectedFactory->getPluginParameters(pluginName.toStdString());
        tlp::Iterator<ParameterDescription> *it = parameters.getParameters();

        while (it->hasNext()) {
            ParameterDescription pd = it->next();
            QString paramName = "\"" + QString(pd.getName().c_str()) + "\" (" + getPythonTypeName(pd.getTypeName().c_str()) + ")";
            paramName.replace("\n", "\\n");

            if (paramName.startsWith(prefix))
                ret.insert(paramName);
        }

        delete it;
    }

    return ret;
}

static QSet<QString> getStringCollectionEntriesForPlugin(const QString &pluginName, const QString &strCollectionName, const QString &prefix="") {
    QSet<QString> ret;
    tlp::TemplateFactoryInterface *selectedFactory = NULL;
    std::map< std::string, tlp::TemplateFactoryInterface* >::iterator it = tlp::TemplateFactoryInterface::allFactories->begin();

    for (; it != tlp::TemplateFactoryInterface::allFactories->end() ; ++it) {
        if (it->second->pluginExists(pluginName.toStdString())) {
            selectedFactory = it->second;
            break;
        }
    }

    if (selectedFactory) {
        const tlp::ParameterDescriptionList&  parameters = selectedFactory->getPluginParameters(pluginName.toStdString());
        tlp::DataSet dataSet;
        parameters.buildDefaultDataSet(dataSet);
        tlp::StringCollection sc;
        dataSet.get(strCollectionName.toStdString(), sc);
        for (size_t i = 0 ; i < sc.size() ; ++i) {
            QString entry = "\"" + QString(sc[i].c_str()) + "\"";
            if (entry.startsWith(prefix))
                ret.insert(entry);
        }
    }

    return ret;
}


static QString getPythonTypeNameForGraphProperty(tlp::Graph *graph, const QString &propName) {
    if (graph->existLocalProperty(propName.toStdString())) {
        PropertyInterface *prop = graph->getProperty(propName.toStdString());
        if (prop->getTypename() == "bool") {
            return "tlp.BooleanProperty";
        } else if (prop->getTypename() == "int") {
            return "tlp.IntegerProperty";
        } else if (prop->getTypename() == "double") {
            return "tlp.DoubleProperty";
        } else if (prop->getTypename() == "color") {
            return "tlp.ColorProperty";
        } else if (prop->getTypename() == "layout") {
            return "tlp.LayoutProperty";
        } else if (prop->getTypename() == "size") {
            return "tlp.SizeProperty";
        } else if (prop->getTypename() == "string") {
            return "tlp.StringProperty";
        } else if (prop->getTypename() == "graph") {
            return "tlp.GraphProperty";
        }
    }

    QString ret = "";

    Graph *sg = NULL;
    forEach(sg, graph->getSubGraphs()) {
        ret = getPythonTypeNameForGraphProperty(sg, propName);
        if (ret != "") {
            return ret;
        }
    }

    return ret;
}

static QString getPythonTypeNameForPropertyType(const QString &propertyType, const bool nodes) {
    if (propertyType == "tlp.BooleanProperty") {
        return "boolean";
    } else if (propertyType == "tlp.LayoutProperty") {
        if (nodes) {
            return "tlp.Coord";
        } else {
            return "list-of-tlp.Coord";
        }
    } else if (propertyType == "tlp.SizeProperty") {
        return "tlp.Size";
    } else if (propertyType == "tlp.ColorProperty") {
        return "tlp.Color";
    } else if (propertyType == "tlp.DoubleProperty") {
        return "float";
    } else if (propertyType == "tlp.IntegerProperty") {
        return "integer";
    } else if (propertyType == "tlp.GraphProperty") {
        if (nodes) {
            return "tlp.Graph";
        } else {
            return "list-of-tlp.edge";
        }
    } else if (propertyType == "tlp.StringProperty") {
        return "string";
    }
    return "";
}

void AutoCompletionDataBase::analyseCurrentScriptCode(const QString &code) {

    globalAutoCompletionList.clear();
    functionAutoCompletionList.clear();
    pluginParametersDataSet.clear();
    varToPluginName.clear();

    varToType.clear();
    varToType["global"] = QHash<QString, QString>();
    varToType["global"]["graph"] = "tlp.Graph";
    varToType["global"]["tlp"] = "tlp";

    globalAutoCompletionList.insert("tlp");

    if (PythonInterpreter::getInstance()->runString("import __builtin__")) {
        std::vector<std::string> builtinDictContent = PythonInterpreter::getInstance()->getObjectDictEntries("__builtin__");
        for (size_t i = 0 ; i < builtinDictContent.size() ; ++i) {
            globalAutoCompletionList.insert(builtinDictContent[i].c_str());
        }
    }

    QString codeCp(code);

    QTextStream in(&codeCp);

    QRegExp importRegexp("^import[ ]+[A-Za-z_][A-Za-z0-9_]*.*$");
    QRegExp globalVarRegexp("^global[ ]+[A-Za-z_][A-Za-z0-9_]*.*$");
    QRegExp varAssignRegexp("^[a-zA-Z_][a-zA-Z0-9_]*[ ]*=[ ]*.*$");
    QRegExp forRegexp("^for[ ]+[a-zA-Z_][a-zA-Z0-9_]*[ ]+in[ ]+.*:$");
    QRegExp funcRegexp("^def[ ]+[A-Za-z_][A-Za-z0-9_]+(.*):$");
    QRegExp classRegexp("^class[ ]+[A-Za-z_][A-Za-z0-9_]*.*:$");
    QRegExp methodCallRegexp("[A-Za-z_][A-Za-z0-9_]*\\.[A-Za-z_][A-Za-z0-9_]*\\(.*\\)");
    QRegExp pluginDataSetRegexp("^[a-zA-Z_][a-zA-Z0-9_]*[ ]*=[ ]*tlp\\.getDefaultPluginParameters\\(.*\\).*$");
    QRegExp graphPropRegexp("\\w+\\[\".+\"\\]");
    QRegExp graphPropAccessRegexp("\\w+\\[\".+\"\\]\\[.+\\]");

    QString currentClassName = "";
    QString currentFunctionName = "global";

    while (!in.atEnd()) {
        QString origLine = in.readLine();
        QString line = origLine.trimmed();

        if (origLine == "" || origLine.startsWith("#"))
            continue;

        if (!(origLine.startsWith("\t") || origLine.startsWith(" "))) {
            currentClassName = "";
            currentFunctionName = "global";
        }

        QString fullName = currentFunctionName;

        if (currentClassName != "") {
            fullName = currentClassName + "." + fullName;
        }


        if (methodCallRegexp.indexIn(line) != -1) {
            QString expr = line.mid(methodCallRegexp.indexIn(line), line.indexOf('(', methodCallRegexp.indexIn(line)) - methodCallRegexp.indexIn(line));
            QStringList parts = expr.split(".");
            QString varName = parts.at(0);
            QVector<QString> types = apiDb->findTypesContainingDictEntry(parts.at(1));

            if (types.size() == 1) {
                if (varToType.find(fullName) == varToType.end()) {
                    varToType[fullName] = QHash<QString, QString>();
                }

                if (varToType[fullName].find(varName) == varToType[fullName].end()) {
                    varToType[fullName][varName] = types.at(0);
                }
            }
        }

        if (varAssignRegexp.indexIn(line) != -1) {
            QString varName = line.mid(0, line.indexOf('=')).trimmed();
            QString expr = line.mid(line.indexOf('=')+1).trimmed();

            QString type = findTypeForExpr(expr, fullName);

            if (type == "") {
                if (expr.length() > 1 && expr[0] == '\"' && expr[expr.length() - 1] == '\"') {
                    type = "string";
                }
                else if (expr.length() > 1 && expr[0] == '[' && expr[expr.length() - 1] == ']') {
                    type = "list";
                }
                else if (expr.length() > 1 && expr[0] == '{' && expr[expr.length() - 1] == '}') {
                    type = "dict";
                }
            }

            if (type != "") {
                if (varToType.find(fullName) == varToType.end()) {
                    varToType[fullName] = QHash<QString, QString>();
                }

                varToType[fullName][varName] = type;
            }

            if (currentFunctionName == "global") {
                globalAutoCompletionList.insert(varName);
            }
            else {
                functionAutoCompletionList[fullName].insert(varName);
            }
        }

        if (pluginDataSetRegexp.indexIn(line) != -1) {
            QString varName = line.mid(0, line.indexOf('=')).trimmed();
            QString pattern = "getDefaultPluginParameters(\"";
            int pos = line.indexOf(pattern);

            if (pos != -1) {
                pos += pattern.length();
                int pos2 = line.indexOf("\"", pos);

                if (pos2 != -1) {
                    QString pluginName = line.mid(pos, pos2-pos);

                    if (tlpPluginExists(pluginName)) {
                        if (pluginParametersDataSet.find(fullName) == pluginParametersDataSet.end()) {
                            pluginParametersDataSet[fullName] = QHash<QString, QSet<QString> >();
                            varToPluginName[fullName] = QHash<QString, QString>();
                        }
                        varToPluginName[fullName][varName] = pluginName;
                        pluginParametersDataSet[fullName][varName] = getParametersListForPlugin(pluginName);
                        foreach(QString param, pluginParametersDataSet[fullName][varName]) {
                            QString name = param.mid(0, param.indexOf("(") - 1);
                            QString type = param.mid(param.indexOf("(")+1, param.indexOf(")") - param.indexOf("(") - 1);
                            QString dataSetVarName = varName + "[" + name + "]";
                            if (varToType.find(fullName) == varToType.end()) {
                                varToType[fullName] = QHash<QString, QString>();
                            }

                            varToType[fullName][dataSetVarName] = type;
                        }
                    }
                }
            }
        }

        if (forRegexp.indexIn(line) != -1) {
            QString varName = line.mid(4).trimmed();
            varName = varName.mid(0, varName.indexOf(QRegExp("\\bin\\b"))).trimmed();

            if (currentFunctionName == "global") {
                globalAutoCompletionList.insert(varName);
            }
            else {
                functionAutoCompletionList[fullName].insert(varName);
            }

            QString expr = line.mid(line.indexOf(QRegExp("\\bin\\b"))+3).trimmed();
            expr = expr.mid(0, expr.indexOf(":")).trimmed();

            QString type = findTypeForExpr(expr, fullName);

            if (iteratorType.find(type) != iteratorType.end()) {
                type = iteratorType[type];
            }

            else if (type.startsWith("list-of-")) {
                type = type.mid(8);

                if (type.startsWith("std_set") || type.startsWith("std_vector") || type.startsWith("std_list")) {
                    type = "list";
                }
            }

            if (type != "") {
                if (varToType.find(fullName) == varToType.end()) {
                    varToType[fullName] = QHash<QString, QString>();
                }

                varToType[fullName][varName] = type;
            }
        }

        if (funcRegexp.indexIn(line) != -1) {
            QString funcName = line.mid(0, line.indexOf('('));
            funcName = funcName.mid(4, line.indexOf('(')).trimmed();
            currentFunctionName = funcName;
            globalAutoCompletionList.insert(funcName);
            QString fullName = currentFunctionName;

            if (currentClassName != "") {
                fullName = currentClassName + "." + fullName;
            }

            if (functionAutoCompletionList.find(fullName) == functionAutoCompletionList.end()) {
                functionAutoCompletionList[fullName] = QSet<QString>();
            }

            QString params = line.mid(line.indexOf('(')+1, line.indexOf(')') - line.indexOf('(') - 1);

            if (params.indexOf(",") == -1) {
                functionAutoCompletionList[fullName].insert(params.trimmed());
            }
            else {
                QStringList pList = params.split(",");
                foreach(QString param, pList) {
                    functionAutoCompletionList[fullName].insert(param.trimmed());
                }
            }
        }

        if (globalVarRegexp.indexIn(line) != -1) {
            QString varName = line.mid(7).trimmed();
            globalAutoCompletionList.insert(varName);
        }

        if (importRegexp.indexIn(line) != -1) {
            QString varName = line.mid(7).trimmed();
            globalAutoCompletionList.insert(varName);
        }

        if (classRegexp.indexIn(line) != -1) {
            QString className = line;

            if (className.indexOf('(') != -1)
                className = className.mid(6, className.indexOf('(')-6);
            else
                className = className.mid(6, className.indexOf(':')-6);

            currentClassName = "";
            globalAutoCompletionList.insert(className.trimmed());
        }



        if (graphPropRegexp.indexIn(line) != -1) {
            QString expr = line.mid(graphPropRegexp.indexIn(line), graphPropRegexp.matchedLength());
            QString varName = expr.mid(0, expr.indexOf("["));
            QString propName = expr.mid(expr.indexOf("\"")+1, expr.lastIndexOf("\"") - expr.indexOf("\"") - 1);

            if (findTypeForExpr(varName, fullName) == "tlp.Graph") {
                QString type = getPythonTypeNameForGraphProperty(graph->getRoot(), propName);
                if (type != "") {
                    if (varToType.find(fullName) == varToType.end()) {
                        varToType[fullName] = QHash<QString, QString>();
                    }

                    varToType[fullName][expr] = type;
                }
            }
        }


        if (graphPropAccessRegexp.indexIn(line) != -1) {
            QString expr = line.mid(graphPropAccessRegexp.indexIn(line), graphPropAccessRegexp.matchedLength());
            int pos = expr.indexOf("[");
            QString varName = expr.mid(0, pos);
            QString propName = expr.mid(expr.indexOf("\"")+1, expr.lastIndexOf("\"") - expr.indexOf("\"") - 1);
            pos = expr.indexOf("[", pos+1);
            QString varName2 = expr.mid(pos+1, expr.lastIndexOf("]") - pos - 1);

            if (findTypeForExpr(varName, fullName) == "tlp.Graph") {
                QString type = getPythonTypeNameForGraphProperty(graph->getRoot(), propName);
                if (type != "") {
                    QString type2 = findTypeForExpr(varName2, fullName);
                    QString type3 = "";
                    if (type2 == "tlp.node") {
                        type3 = getPythonTypeNameForPropertyType(type, true);
                    } else if (type2 == "tlp.edge") {
                        type3 = getPythonTypeNameForPropertyType(type, false);
                    }

                    if (type3 != "") {
                        if (varToType.find(fullName) == varToType.end()) {
                            varToType[fullName] = QHash<QString, QString>();
                        }

                        varToType[fullName][expr] = type3;
                    }
                }
            }
        }
    }
}

QString AutoCompletionDataBase::findTypeForExpr(const QString &expr, const QString &funcName) const {
    QString currentType = "";

    if (expr.indexOf('(') != -1 && expr.indexOf(')') != -1) {
        QString name = expr.mid(0, expr.indexOf('('));

        if (!name.startsWith("tlp.")) {
            name = "tlp." + funcName;
        }

        if (apiDb->typeExists(name)) {
            currentType = name;
        }
        else {
            currentType = apiDb->getReturnTypeForMethodOrFunction(name);
        }
    }

    if (currentType == "" && expr.indexOf(".") != -1) {
        QStringList parts = expr.split(".");
        int i = 0;
        foreach(QString p, parts) {
            if (i==0) {
                if (varToType.find(funcName) != varToType.end()) {
                    if (varToType[funcName].find(p) != varToType[funcName].end()) {
                        currentType = varToType[funcName][p];
                        ++i;
                        continue;
                    }
                }

                if (apiDb->getDictContentForType(p).count() > 0) {
                    currentType = p;
                }
                else if (varToType["global"].find(p) != varToType["global"].end()) {
                    currentType = varToType["global"][p];
                }

                if (currentType == "") {
                    currentType = PythonInterpreter::getInstance()->getVariableType(p.toStdString()).c_str();
                }

            }
            else {
                if (p.indexOf('(') != -1 && p.indexOf(')') != -1) {
                    QString func = p.mid(0, p.indexOf('('));
                    currentType = apiDb->getReturnTypeForMethodOrFunction(currentType + "." + func);
                }
                else if (apiDb->getDictContentForType(currentType + "." + p).count() > 0) {
                    currentType = currentType + "." + p;
                }
                else if (apiDb->dictEntryExists(currentType, p)) {
                    currentType = currentType + "." + p;
                }
            }

            ++i;

            if (currentType == "")
                break;

        }
    }
    else {
        if (varToType[funcName].find(expr) != varToType[funcName].end()) {
            currentType = varToType[funcName][expr];
        }
        else if (varToType["global"].find(expr) != varToType["global"].end()) {
            currentType = varToType["global"][expr];
        }

        if (currentType == "") {
            currentType = PythonInterpreter::getInstance()->getVariableType(expr.toStdString()).c_str();
        }
    }

    return currentType;
}

static QVector<PropertyInterface*> getAllGraphPropertiesFromRoot(Graph *root) {
    QVector<PropertyInterface*> ret;
    string prop;
    forEach(prop, root->getLocalProperties()) {
        ret.append(root->getProperty(prop));
    }
    Graph *sg=NULL;
    forEach(sg, root->getSubGraphs()) {
        ret += getAllGraphPropertiesFromRoot(sg);
    }
    return ret;
}

static QSet<QString> getAllSubGraphsNamesFromRoot(Graph *root, const QString &prefix) {
    QSet<QString> ret;
    tlp::Graph *sg=NULL;
    forEach(sg, root->getSubGraphs()) {
        QString sgName = "\"" + QString(sg->getName().c_str()) + "\"";
        if (sgName.startsWith(prefix))
            ret.insert(sgName);
    }
    forEach(sg, root->getSubGraphs()) {
        ret += getAllSubGraphsNamesFromRoot(sg, prefix);
    }
    return ret;
}

static QSet<QString> getGraphPropertiesList(Graph *graph, const QString &prefix, const QString &type="") {
    QSet<QString> ret;
    string prop;
    QVector<PropertyInterface*> properties = getAllGraphPropertiesFromRoot(graph);
    foreach(PropertyInterface* prop, properties) {
        if (type == "" || prop->getTypename() == type.toStdString()) {
            QString qProp = "\"" + QString(prop->getName().c_str()) + "\"";

            if (qProp.startsWith(prefix)) {
                ret.insert(qProp);
            }
        }
    }
    return ret;
}

QSet<QString> AutoCompletionDataBase::getPluginParametersListIfContext(const QString &context, const QString &editedFunction) const {
    QSet<QString> ret;

    if (pluginParametersDataSet.find(editedFunction) != pluginParametersDataSet.end()) {
        int pos = context.lastIndexOf("[");
        QString varName = context.mid(0, pos);
        QString strCollecExpr = "].setCurrent(";
        int pos2 = context.indexOf(strCollecExpr, pos+1);
        if (pos != -1 && pos2 == -1) {
            QString prefix = context.mid(pos+1);

            if (pluginParametersDataSet[editedFunction].find(varName) != pluginParametersDataSet[editedFunction].end()) {
                foreach(QString param, pluginParametersDataSet[editedFunction][varName]) {
                    if (param.startsWith(prefix)) {
                        ret.insert(param);
                    }
                }
            }

        } else if (pos != -1 && pos2 != -1) {
            QString entryName = context.mid(pos+1, pos2 - pos - 1);
            entryName.replace("\"", "");
            QString prefix = context.mid(pos2+strCollecExpr.size());
            if (pluginParametersDataSet[editedFunction].find(varName) != pluginParametersDataSet[editedFunction].end()) {
                foreach(QString param, pluginParametersDataSet[editedFunction][varName]) {
                    if (param.indexOf(entryName) != -1 && param.indexOf("tlp.StringCollection") != -1) {
                        ret = getStringCollectionEntriesForPlugin(varToPluginName[editedFunction][varName], entryName, prefix);
                    }
                }
            }
        }
    }

    return ret;
}

QSet<QString> AutoCompletionDataBase::getSubGraphsListIfContext(const QString &context, const QString &editedFunction) const  {

    QString cleanContext = context;
    QSet<QString> ret;

    QString sgExpr = ".getSubGraph(";

    if (graph && cleanContext.lastIndexOf(sgExpr) != -1) {
        int i = 0;

        while (sepChar[i]) {
            if (sepChar[i] != '(' && cleanContext.lastIndexOf(sepChar[i]) != -1) {
                cleanContext = cleanContext.mid(cleanContext.lastIndexOf(sepChar[i])+1);
            }

            ++i;
        }

        QString expr = cleanContext.mid(0, cleanContext.lastIndexOf(sgExpr));
        QString type = findTypeForExpr(expr, editedFunction);

        if (type == "tlp.Graph") {
            QString prefix = cleanContext.mid(cleanContext.lastIndexOf(sgExpr)+sgExpr.size());
            ret = getAllSubGraphsNamesFromRoot(graph->getRoot(), prefix);
        }
    }
    return ret;
}

QSet<QString> AutoCompletionDataBase::getGraphPropertiesListIfContext(const QString &context, const QString &editedFunction) const  {

    QString cleanContext = context;
    QSet<QString> ret;

    if (graph && cleanContext.lastIndexOf("[") != -1) {
        int i = 0;

        while (sepChar[i]) {
            if (sepChar[i] != '[' && cleanContext.lastIndexOf(sepChar[i]) != -1) {
                cleanContext = cleanContext.mid(cleanContext.lastIndexOf(sepChar[i])+1);
            }

            ++i;
        }

        QString expr = cleanContext.mid(0, cleanContext.lastIndexOf("["));
        QString type = findTypeForExpr(expr, editedFunction);

        if (type == "tlp.Graph") {
            QString prefix = cleanContext.mid(cleanContext.lastIndexOf("[")+1);
            ret = getGraphPropertiesList(graph->getRoot(), prefix);
        }
    } else if (graph && cleanContext.lastIndexOf("(") != -1) {
        int i = 0;

        while (sepChar[i]) {
            if (sepChar[i] != '(' && cleanContext.lastIndexOf(sepChar[i]) != -1) {
                cleanContext = cleanContext.mid(cleanContext.lastIndexOf(sepChar[i])+1);
            }

            ++i;
        }

        QString expr = cleanContext.mid(0, cleanContext.lastIndexOf("("));
        QString prefix = cleanContext.mid(cleanContext.lastIndexOf("(")+1);
        QString type = findTypeForExpr(expr, editedFunction);

        if (type == "tlp.Graph.getBooleanProperty" || type == "tlp.Graph.getLocalBooleanProperty") {
            ret = getGraphPropertiesList(graph->getRoot(), prefix, "bool");
        }

        if (type == "tlp.Graph.getColorProperty" || type == "tlp.Graph.getLocalColorProperty") {
            ret = getGraphPropertiesList(graph->getRoot(), prefix, "color");
        }

        if (type == "tlp.Graph.getDoubleProperty" || type == "tlp.Graph.getLocalDoubleProperty") {
            ret = getGraphPropertiesList(graph->getRoot(), prefix, "double");
        }

        if (type == "tlp.Graph.getGraphProperty" || type == "tlp.Graph.getLocalGraphProperty") {
            ret = getGraphPropertiesList(graph->getRoot(), prefix, "graph");
        }

        if (type == "tlp.Graph.getIntegerProperty" || type == "tlp.Graph.getLocalIntegerProperty") {
            ret = getGraphPropertiesList(graph->getRoot(), prefix, "int");
        }

        if (type == "tlp.Graph.getLayoutProperty" || type == "tlp.Graph.getLocalLayoutProperty") {
            ret = getGraphPropertiesList(graph->getRoot(), prefix, "layout");
        }

        if (type == "tlp.Graph.getSizeProperty" || type == "tlp.Graph.getLocalSizeProperty") {
            ret = getGraphPropertiesList(graph->getRoot(), prefix, "size");
        }

        if (type == "tlp.Graph.getStringProperty" || type == "tlp.Graph.getLocalStringProperty") {
            ret = getGraphPropertiesList(graph->getRoot(), prefix, "string");
        }
    }

    return ret;
}

static QSet<QString> getAlgorithmPluginsListOfType(const QString& type, const QString &prefix) {
    QSet<QString> ret;
    tlp::Iterator<tlp::AlgorithmPlugin *> *itP = tlp::AlgorithmPlugin::factory->availablePluginObjects();

    while (itP->hasNext()) {
        tlp::AlgorithmPlugin *plugin = itP->next();

        if (type.toStdString() == plugin->getClassName() || type.isEmpty()) {
            QString pluginName = "\"" + QString(plugin->getName().c_str()) + "\"";

            if (pluginName.startsWith(prefix))
                ret.insert(pluginName);
        }
    }

    delete itP;
    return ret;
}

static QSet<QString> tryAlgorithmContext(const QString &context, const QString &algoContext, const QString &algoType="") {
    QSet<QString> ret;

    if (context.indexOf(algoContext) != -1) {
        int pos1 = context.indexOf(algoContext);
        int pos2 = pos1 + algoContext.length();

        if (context.indexOf(",", pos2) == -1 && (algoType.isEmpty() || context.mid(0, pos1) != "tlp")) {
            QString prefix = context.mid(pos2);
            ret = getAlgorithmPluginsListOfType(algoType, prefix);
        }
    }

    return ret;
}

static QSet<QString> getTulipAlgorithmListIfContext(const QString &context) {
    QSet<QString> ret;
    ret = tryAlgorithmContext(context, ".applyAlgorithm(", "Algorithm");

    if (!ret.empty()) {
        return ret;
    }

    ret = tryAlgorithmContext(context, ".getDefaultPluginParameters(");

    if (!ret.empty()) {
        return ret;
    }

    ret = tryAlgorithmContext(context, ".computeBooleanProperty(", "Boolean");

    if (!ret.empty()) {
        return ret;
    }

    ret = tryAlgorithmContext(context, ".computeColorProperty(", "Color");

    if (!ret.empty()) {
        return ret;
    }

    ret = tryAlgorithmContext(context, ".computeDoubleProperty(", "Double");

    if (!ret.empty()) {
        return ret;
    }

    ret = tryAlgorithmContext(context, ".computeIntegerProperty(", "Integer");

    if (!ret.empty()) {
        return ret;
    }

    ret = tryAlgorithmContext(context, ".computeLayoutProperty(", "Layout");

    if (!ret.empty()) {
        return ret;
    }

    ret = tryAlgorithmContext(context, ".computeSizeProperty(", "Size");

    if (!ret.empty()) {
        return ret;
    }

    ret = tryAlgorithmContext(context, ".computeStringProperty(", "String");

    return ret;
}

QSet<QString> AutoCompletionDataBase::getAutoCompletionListForContext(const QString &context, const QString &editedFunction) const {

    QSet<QString> ret;

    QString cleanContext = context;

    ret = getTulipAlgorithmListIfContext(cleanContext);

    if (!ret.empty()) {
        return ret;
    }

    ret = getGraphPropertiesListIfContext(cleanContext, editedFunction);

    if (!ret.empty()) {
        return ret;
    }

    ret = getPluginParametersListIfContext(cleanContext, editedFunction);

    if (!ret.empty()) {
        return ret;
    }

    ret = getSubGraphsListIfContext(cleanContext, editedFunction);

    if (!ret.empty()) {
        return ret;
    }

    int i = 0;

    while (sepChar[i]) {
        int pos = cleanContext.lastIndexOf(sepChar[i]);
        if (pos != -1) {
            if ((!(sepChar[i] == '[' && pos < cleanContext.size()-1 && cleanContext[pos+1] == '"')) &&
                    (!(sepChar[i] == '[' && cleanContext.indexOf(']', pos+1) != -1))) {
                int nbQuotes = 0;
                int nbDblQuotes = 0;
                for (int j = pos-1 ; j >= 0 ; --j) {
                    if (cleanContext[j] == '\'') {
                        ++nbQuotes;
                    }
                    if (cleanContext[j] == '"') {
                        ++nbDblQuotes;
                    }
                }
                if (nbQuotes%2 == 0 && nbDblQuotes%2 == 0) {
                    cleanContext = cleanContext.mid(cleanContext.lastIndexOf(sepChar[i])+1);
                }
            }
        }

        ++i;
    }

    if (cleanContext.indexOf('.') == -1) {

        foreach(QString s, globalAutoCompletionList) {
            if (s.startsWith(cleanContext)) {
                ret.insert(s);
            }
        }

        if (functionAutoCompletionList.find(editedFunction) != functionAutoCompletionList.end()) {
            foreach(QString s, functionAutoCompletionList[editedFunction]) {
                if (s.startsWith(cleanContext)) {
                    ret.insert(s);
                }
            }
        }

    }
    else {

        QString type = findTypeForExpr(cleanContext.mid(0, cleanContext.lastIndexOf('.')+1), editedFunction);
        QString prefix = cleanContext.mid(cleanContext.lastIndexOf('.')+1);
        QSet<QString> dict;



        if (type.startsWith("list")) {
            type = "list";
        }
        else if (type.startsWith("dict")) {
            type = "dict";
        }

        if (type != "") {
            ret = apiDb->getDictContentForType(type, prefix);
        }
        else {
            ret = apiDb->getAllDictEntriesStartingWithPrefix(prefix);
        }
    }

    return ret;
}
