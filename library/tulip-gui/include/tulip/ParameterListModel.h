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
#ifndef PARAMETERLISTMODEL_H
#define PARAMETERLISTMODEL_H

#include "tulip/TulipModel.h"
#include <tulip/WithParameter.h>
#include <tulip/DataSet.h>
#include <tulip/TulipMetaTypes.h>

namespace tlp {

class TLP_QT_SCOPE ParameterListModel : public TulipModel {
  struct ParamInfos {
    inline ParamInfos(bool m,const QString &n,const QString &d, const std::string& t): mandatory(m), name(n),desc(d), type(t) {}
    bool mandatory;
    QString name;
    QString desc;
    std::string type;
  };

  struct ParamInfosSorter {
    bool operator()(ParamInfos a, ParamInfos b);
  };

  std::vector<ParamInfos> _params;
  tlp::DataSet _data;
  tlp::Graph* _graph;

public:
  explicit ParameterListModel(const tlp::ParameterDescriptionList &params, tlp::Graph *graph=0, QObject *parent=0);
  tlp::DataSet parametersValues() const;

  QModelIndex index(int row, int column,const QModelIndex &parent = QModelIndex()) const;
  QModelIndex parent(const QModelIndex &child) const;
  int rowCount(const QModelIndex &parent = QModelIndex()) const;
  int columnCount(const QModelIndex &parent = QModelIndex()) const;
  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
  QVariant headerData(int section, Qt::Orientation orientation, int role) const;
  Qt::ItemFlags flags(const QModelIndex &index) const;
  bool setData(const QModelIndex &index, const QVariant &value, int role);
};

}

#endif // PARAMETERLISTMODEL_H