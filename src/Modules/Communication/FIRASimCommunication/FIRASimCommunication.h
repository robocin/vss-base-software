//
// Created by jose-cruz on 09/02/2021.
//

#ifndef VSS_BASE_SOFTWARE_FIRASIMCOMMUNICATION_H
#define VSS_BASE_SOFTWARE_FIRASIMCOMMUNICATION_H

#include <QUdpSocket>
#include <QNetworkInterface>
#include "Modules/Modules.h"
#include "SimulationUtils/SimulationUtils.h"

class FIRASimCommunication : public CommunicationBase {
 public:
  explicit FIRASimCommunication(QThreadPool* threadPool);

 protected:
  void buildParameters(Parameters::Handler& parameters) override;
  void connectModules(const Modules* modules) override;
  void init(const Modules* modules) override;
  void update() override;
  void exec() override;

  struct Args {
    Parameters::Arg<QString> ip = "127.0.0.1";
    Parameters::Arg<int> port = 20011;

    Parameters::Arg<bool> replacement = true;
    Parameters::Arg<float> replacementAngle = 0;
  };
  Args args;

  struct Shared {
    SharedOptional<bool> hasIsYellow;
    SharedValue<QVector<Navigation::Output>> packets;

    SharedOptional<Point> replacementPos;
    SharedValue<QSet<Qt::Key>> pressedKeys;
  };
  SharedWrapper<Shared, std::mutex> shared;

  std::unique_ptr<QUdpSocket, DeleteLaterDeleter> socket;

  std::optional<bool> hasIsYellow;
  QVector<Navigation::Output> packets;

  void setupClient();

 private slots:
  void receiveRelativeMousePos(const Point& position);
  void receiveKeyPress(Qt::Key key);
  void receiveKeyRelease(Qt::Key key);

  void receiveIsYellow(bool isYellow);
  void receiveNavigation(const Navigation::Output& navigation);

 private:
  static std::optional<Point> isBallReplacement(const Shared& obj);
  static std::optional<std::tuple<bool, int, Point>> isRobotReplacement(const Shared& obj);
};

#endif // VSS_BASE_SOFTWARE_FIRASIMCOMMUNICATION_H
