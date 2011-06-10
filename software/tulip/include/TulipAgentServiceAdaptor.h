/*
 * This file was generated by qdbusxml2cpp version 0.7
 * Command line was: qdbusxml2cpp -c TulipAgentService -a include/TulipAgentServiceAdaptor.h:src/TulipAgentServiceAdaptor.cpp resources/org.labri.TulipAgentService.xml
 *
 * qdbusxml2cpp is Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
 *
 * This is an auto-generated file.
 * This file may have been hand-edited. Look for HAND-EDIT comments
 * before re-generating it.
 */

#ifndef TULIPAGENTSERVICEADAPTOR_H_1307704634
#define TULIPAGENTSERVICEADAPTOR_H_1307704634

#include <QtCore/QObject>
#include <QtDBus/QtDBus>
class QByteArray;
template<class T> class QList;
template<class Key, class Value> class QMap;
class QString;
class QStringList;
class QVariant;

/*
 * Adaptor class for interface org.labri.Tulip
 */
class TulipAgentService: public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.labri.Tulip")
    Q_CLASSINFO("D-Bus Introspection", ""
"  <interface name=\"org.labri.Tulip\">\n"
"    <property access=\"read\" type=\"x\" name=\"pid\"/>\n"
"    <method name=\"ShowWelcomeScreen\"/>\n"
"    <method name=\"ShowPluginsCenter\"/>\n"
"    <method name=\"ShowAboutPage\"/>\n"
"    <method name=\"ShowOpenProjectWindow\"/>\n"
"    <method name=\"OpenProject\">\n"
"      <arg direction=\"in\" type=\"s\" name=\"file\"/>\n"
"    </method>\n"
"    <method name=\"OpenProjectWith\">\n"
"      <arg direction=\"in\" type=\"s\" name=\"file\"/>\n"
"      <arg direction=\"in\" type=\"s\" name=\"perspective\"/>\n"
"    </method>-->\n"
"    <method name=\"CreatePerspective\">\n"
"      <arg direction=\"in\" type=\"s\" name=\"name\"/>\n"
"    </method>\n"
"    <method name=\"AddPluginRepository\">\n"
"      <arg direction=\"in\" type=\"s\" name=\"url\"/>\n"
"    </method>\n"
"    <method name=\"RemovePluginRepository\">\n"
"      <arg direction=\"in\" type=\"s\" name=\"url\"/>\n"
"    </method>\n"
"    <method name=\"GetCompatiblePerspectives\">\n"
"      <arg direction=\"in\" type=\"s\" name=\"file\"/>\n"
"      <arg direction=\"out\" type=\"as\" name=\"result\"/>\n"
"    </method>\n"
"    <signal name=\"Terminate\"/>\n"
"  </interface>\n"
        "")
public:
    TulipAgentService(QObject *parent);
    virtual ~TulipAgentService();

public: // PROPERTIES
    Q_PROPERTY(qlonglong pid READ pid)
    qlonglong pid() const;

public Q_SLOTS: // METHODS
    void AddPluginRepository(const QString &url);
    void CreatePerspective(const QString &name);
    QStringList GetCompatiblePerspectives(const QString &file);
    void OpenProject(const QString &file);
    void OpenProjectWith(const QString &file, const QString &perspective);
    void RemovePluginRepository(const QString &url);
    void ShowAboutPage();
    void ShowOpenProjectWindow();
    void ShowPluginsCenter();
    void ShowWelcomeScreen();
Q_SIGNALS: // SIGNALS
    void Terminate();
};

#endif
