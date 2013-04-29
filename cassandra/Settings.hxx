#ifndef SETTINGS_H
#define SETTINGS_H

#include <QMainWindow>

namespace Ui {
class Settings;
}

class Settings : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit Settings(QWidget *parent = 0, int LOD=0);
    ~Settings();
    
private:
    Ui::Settings *ui;

public slots:
    void LODmodified(int LOD);

signals:
    void LODmodif(int LOD);


};

#endif // SETTINGS_H
