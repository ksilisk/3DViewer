#include "mainwindow.h"

#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);

  // SpinBoxes
  connect(ui->spinPosX, SIGNAL(valueChanged(double)), this,
          SLOT(updateModelPosition()));
  connect(ui->spinPosY, SIGNAL(valueChanged(double)), this,
          SLOT(updateModelPosition()));
  connect(ui->spinPosZ, SIGNAL(valueChanged(double)), this,
          SLOT(updateModelPosition()));
  connect(ui->spinRotX, SIGNAL(valueChanged(double)), this,
          SLOT(updateModelRotation()));
  connect(ui->spinRotY, SIGNAL(valueChanged(double)), this,
          SLOT(updateModelRotation()));
  connect(ui->spinRotZ, SIGNAL(valueChanged(double)), this,
          SLOT(updateModelRotation()));
  connect(ui->spinScX, SIGNAL(valueChanged(double)), this,
          SLOT(updateModelScale()));
  connect(ui->spinScY, SIGNAL(valueChanged(double)), this,
          SLOT(updateModelScale()));
  connect(ui->spinScZ, SIGNAL(valueChanged(double)), this,
          SLOT(updateModelScale()));

  // Model Loading
  connect(ui->buttonBrowse, SIGNAL(clicked()), this, SLOT(browsePath()));
  connect(ui->buttonLoad, SIGNAL(clicked()), this, SLOT(loadModel()));

  // Projection radios
  connect(ui->radioProjC, SIGNAL(clicked()), this, SLOT(updateProjection()));
  connect(ui->radioProjP, SIGNAL(clicked()), this, SLOT(updateProjection()));

  // Rendering Params (colors, thickness)
  connect(ui->buttonBgColor, SIGNAL(clicked()), this, SLOT(browseBgColor()));
  connect(ui->editBgColor, SIGNAL(returnPressed()), this,
          SLOT(updateBgColor()));
  connect(ui->buttonLineColor, SIGNAL(clicked()), this,
          SLOT(browseLineColor()));
  connect(ui->editLineColor, SIGNAL(returnPressed()), this,
          SLOT(updateLineColor()));
  connect(ui->comboLineType, SIGNAL(activated(int)), this,
          SLOT(updateLineType()));
  connect(ui->spinLineThick, SIGNAL(valueChanged(double)), this,
          SLOT(updateLineThick()));
  connect(ui->buttonPointColor, SIGNAL(clicked()), this,
          SLOT(browsePointColor()));
  connect(ui->editPointColor, SIGNAL(returnPressed()), this,
          SLOT(updatePointColor()));
  connect(ui->comboPointType, SIGNAL(activated(int)), this,
          SLOT(updatePointType()));
  connect(ui->spinPointSize, SIGNAL(valueChanged(double)), this,
          SLOT(updatePointSize()));

  // Setup comboboxes
  ui->comboLineType->addItem("Solid");
  ui->comboLineType->addItem("Dashed");
  ui->comboLineType->addItem("Dotted");
  ui->comboPointType->addItem("None");
  ui->comboPointType->addItem("Square");
  ui->comboPointType->addItem("Circle");

  // Set up some values for QSettings
  QCoreApplication::setOrganizationName("3DBiber");
  QCoreApplication::setApplicationName("3D Viewer");
  QCoreApplication::setOrganizationDomain("3dviewer");

  loadSettings();
  spawnViewer();
  loadModel();
}

MainWindow::~MainWindow() {
  saveSettings();
  delete ui;
}

void MainWindow::spawnViewer() {
  // Spawn Viewer
  viewerWin = new ViewerWindow(this->windowHandle());
  viewerWin->resize(920, 640);
  viewerWin->show();

  // Call all updates
  updateModelPosition();
  updateModelRotation();
  updateModelScale();
  updateBgColor();
  updateLineColor();
  updateLineType();
  updateLineThick();
  updatePointColor();
  updatePointType();
  updatePointSize();

  // updateProjection(); // doesnt work at start
  viewerWin->projectionType = ui->radioProjC->isChecked()
                                  ? ProjectionType::Orthographic
                                  : ProjectionType::Perspective;
}

void MainWindow::updateModelPosition() {
  viewerWin->position.setX(ui->spinPosX->value());
  viewerWin->position.setY(ui->spinPosY->value());
  viewerWin->position.setZ(ui->spinPosZ->value());
}
void MainWindow::updateModelRotation() {
  viewerWin->rotation.setX(ui->spinRotX->value());
  viewerWin->rotation.setY(ui->spinRotY->value());
  viewerWin->rotation.setZ(ui->spinRotZ->value());
}
void MainWindow::updateModelScale() {
  viewerWin->scale.setX(ui->spinScX->value());
  viewerWin->scale.setY(ui->spinScY->value());
  viewerWin->scale.setZ(ui->spinScZ->value());
}

void MainWindow::loadModel() {
  QString path = ui->editPath->text();
  if (!viewerWin) spawnViewer();
  if (!viewerWin->isExposed()) viewerWin->show();
  viewerWin->load_model(path);
}
void MainWindow::browsePath() {
  QString filePath = QFileDialog::getOpenFileName(
      this, "Choose model", QDir::homePath(), "Model files (*.obj)");
  ui->editPath->setText(filePath);
}

void MainWindow::updateProjection() {
  viewerWin->projectionType = ui->radioProjC->isChecked()
                                  ? ProjectionType::Orthographic
                                  : ProjectionType::Perspective;
  viewerWin->update_projection();
}

void MainWindow::browseBgColor() {
  QColor init = QColor(ui->editBgColor->text());
  QColor color = QColorDialog::getColor(init, this, "Select background color",
                                        QColorDialog::ColorDialogOptions(0));
  ui->editBgColor->setText(color.name());
  viewerWin->backgroundColor = color;
}

void MainWindow::updateBgColor() {
  QColor color = QColor(ui->editBgColor->text());
  viewerWin->backgroundColor = color;
}

void MainWindow::browseLineColor() {
  QColor init = QColor(ui->editLineColor->text());
  QColor color = QColorDialog::getColor(init, this, "Select line color",
                                        QColorDialog::ColorDialogOptions(0));
  ui->editLineColor->setText(color.name());
  viewerWin->lineColor = color;
}

void MainWindow::updateLineColor() {
  QColor color = QColor(ui->editLineColor->text());
  viewerWin->lineColor = color;
}

void MainWindow::browsePointColor() {
  QColor init = QColor(ui->editPointColor->text());
  QColor color = QColorDialog::getColor(init, this, "Select point color",
                                        QColorDialog::ColorDialogOptions(0));
  ui->editPointColor->setText(color.name());
  viewerWin->pointColor = color;
}

void MainWindow::updatePointColor() {
  QColor color = QColor(ui->editPointColor->text());
  viewerWin->pointColor = color;
}

void MainWindow::updateLineType() {
  switch (ui->comboLineType->currentIndex()) {
    case 0:
      viewerWin->lineType = LineType::Solid;
      break;
    case 1:
      viewerWin->lineType = LineType::Dashed;
      break;
    default:
      viewerWin->lineType = LineType::Dotted;
      break;
  }
}

void MainWindow::updateLineThick() {
  viewerWin->lineWidth = (float)ui->spinLineThick->value();
}

void MainWindow::updatePointType() {
  switch (ui->comboPointType->currentIndex()) {
    case 0:
      viewerWin->pointType = PointType::None;
      break;
    case 1:
      viewerWin->pointType = PointType::Square;
      break;
    default:
      viewerWin->pointType = PointType::Circle;
      break;
  }
}

void MainWindow::updatePointSize() {
  viewerWin->pointSize = (float)ui->spinPointSize->value();
}

void MainWindow::saveSettings() {
  auto settings = QSettings();

  settings.setValue("modelpath", ui->editPath->text());

  settings.setValue("bgcolor", ui->editBgColor->text());
  settings.setValue("projectionCentral", ui->radioProjC->isChecked());

  settings.setValue("linecolor", ui->editLineColor->text());
  settings.setValue("linewidth", QString::number(ui->spinLineThick->value()));
  settings.setValue("linetype", ui->comboLineType->currentIndex());

  settings.setValue("pointcolor", ui->editPointColor->text());
  settings.setValue("pointsize", QString::number(ui->spinPointSize->value()));
  settings.setValue("pointtype", ui->comboPointType->currentIndex());

  settings.sync();

  qDebug() << "saved settings";
}

void MainWindow::loadSettings() {
  auto settings = QSettings();

  ui->editPath->setText(settings.value("modelpath", "").toString());

  ui->editBgColor->setText(settings.value("bgcolor", "#000000").toString());
  ui->radioProjC->setChecked(
      settings.value("projectionCentral", false).toBool());
  ui->radioProjP->setChecked(
      !settings.value("projectionCentral", false).toBool());

  ui->editLineColor->setText(settings.value("linecolor", "#FFFFFF").toString());
  ui->comboLineType->setCurrentIndex(settings.value("linetype", 0).toInt());

  ui->editPointColor->setText(
      settings.value("pointcolor", "#FFFFFF").toString());
  ui->comboPointType->setCurrentIndex(settings.value("pointtype", 0).toInt());

  qDebug() << "loaded settings";
}
