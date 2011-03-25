#include <QThread>
#include <QDebug>


#define True 1
#define False 0
class Caller : public QObject {
  Q_OBJECT
  public:
  void interrupt_thread() {
    emit signal_thread();
  }
  signals:
  void signal_thread();
};
class waiter_threads : public QThread {
  Q_OBJECT
  private:
  int stop;
  private slots:
  void interrupted() {
    qDebug() << "ISR";
    stop = True;
  }
public:
  void download();
  void run();
  waiter_threads(Caller *controller) {
    stop = False;
    QObject::connect(controller,SIGNAL(signal_thread()),this,SLOT(interrupted()));
  }
};
