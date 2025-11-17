#include "controller/MainController.hpp"
#include "model/TableModel.hpp"
#include "view/MainWindow.hpp"
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    qRegisterMetaType<DataObject>("DataObject");
    qRegisterMetaType<QVector<DataObject>>("QVector<DataObject>");

    MainWindow view;
    TableModel model;
    MainController controller(&view, &model);

    view.show();
    return app.exec();
}
