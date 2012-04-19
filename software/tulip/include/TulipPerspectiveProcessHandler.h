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
#ifndef TULIPPERSPECTIVEPROCESSHANDLER_H
#define TULIPPERSPECTIVEPROCESSHANDLER_H

#include <QtCore/QObject>
#include <QtCore/QVariantMap>
#include <QtCore/QProcess>

#include <QtGui/QPushButton>


// Some utility class used in crash handler dialog designer form
class SelectionButton: public QPushButton {
public:
  explicit SelectionButton(QWidget *parent=0);
  void paintEvent(QPaintEvent *e);
};

struct PerspectiveProcessInfos {
  PerspectiveProcessInfos() {}

  PerspectiveProcessInfos(const QString name, const QVariantMap &args, const QString &file):
    name(name), args(args), file(file) {
  }

  QString name;
  QVariantMap args;
  QString file;
  QString projectPath;
};

class TulipPerspectiveProcessHandler: public QObject {
  Q_OBJECT

  QMap<QProcess *, PerspectiveProcessInfos> _processInfos;

  static TulipPerspectiveProcessHandler *_instance;
  TulipPerspectiveProcessHandler();

public:
  static TulipPerspectiveProcessHandler &instance();

public slots:
  void createPerspective(const QString &perspective, const QString &file, const QVariantMap &parameters);
  void enableCrashHandling(qlonglong perspectivePid, const QString &perspectiveProjectPath);

protected slots:
  void perspectiveCrashed(QProcess::ProcessError);
  void perspectiveFinished(int exitCode, QProcess::ExitStatus exitStatus);

};

#endif // TULIPPERSPECTIVEPROCESSHANDLER_H