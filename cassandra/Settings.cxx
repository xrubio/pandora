#include "Settings.hxx"
#include "ui_Settings.h"

Settings::Settings(QWidget *parent, int LOD) :
    QMainWindow(parent),
    ui(new Ui::Settings)
{
    ui->setupUi(this);
    ui->LODspinBox->setValue(LOD);
}

Settings::~Settings()
{
    delete ui;
}

void Settings::LODmodified(int LOD)
{
    emit LODmodif(LOD);
}

void Settings::Radiusmodified(int r)
{
    emit Radiusmodif(r);
}

void Settings::Offsetmodified(int o)
{
    emit Offsetmodif(o);
}
