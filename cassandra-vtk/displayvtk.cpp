#include "displayvtk.h"
#include "ui_displayvtk.h"


#include <iostream>
#include <GL/gl.h>
#include <GL/glu.h>
#include <QListWidgetItem>

#include <Exceptions.hxx>
#include <SimulationRecord.hxx>
#include <Model3D.hxx>
#include <Point2D.hxx>
#include <QtGui>
#include <AgentConfiguration.hxx>
#include <RasterConfiguration.hxx>
#include <Configuration3D.hxx>
#include <ColorSelector.hxx>
#include <StaticRaster.hxx>
#include <ProjectConfiguration.hxx>
#include <algorithm>

DisplayVTK::DisplayVTK(QWidget *parent) : QWidget(parent), _simulationRecord(0), _viewedStep(0), _zoom(1.0f), _position(0,0), _lastPos(0,0), _rotation(0,0), _rotationZ(0),  _cellScale(1.0f, 1.0f, 1.0f), _config3D(ProjectConfiguration::instance()->getConfig3D()), ui(new Ui::DisplayVTK)
{
    ui->setupUi(this);
}

DisplayVTK::~DisplayVTK()
{
    delete ui;
}


void DisplayVTK::initializeGL()
{
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    /*
    //Definim variables d'il·luminació i material
    float light_ambient[] = {1.0, 1.0, 1.0, 1.0};
    float light_color[] = {1.0, 1.0, 1.0};
    float light_direction[] = {-1.0, -1.0, -1.0, 0.0};

    float spot_cutoff = 180.0;
    float spot_exponent = 1.0;
    float spot_dir[] = {-1.0, -1.0, -1.0};
    float spot_color[] = {1.0, 1.0, 1.0};

    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_COLOR_MATERIAL);

    //Tipus d'il·luminació que li volem donar a l'escena
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_color);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_color);

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, light_ambient);

// 	Tipus d'il·luminació amb un SPOT
    glLightfv(GL_LIGHT1, GL_DIFFUSE, spot_color);
    glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, spot_cutoff);
    glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, spot_exponent);
    //glEnable -> "encèn" (Recordatori: OpenGL es com una màquina d'estats)
        //Iluminació disponible
    */

        //MOD
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);   //netejar vista


        Engine::StaticRaster & DEMRaster = _simulationRecord->getRasterTmp(_config3D.getDEMRaster(), _viewedStep);

        float puntMig = sqrt(DEMRaster.getSize()._x*DEMRaster.getSize()._x+DEMRaster.getSize()._y*DEMRaster.getSize()._y);

        radi = (puntMig)/2.f; //mida escenari/2
        VRP.x = DEMRaster.getSize()._x/2;
        VRP.y = -DEMRaster.getSize()._y/2;
        VRP.z = 0;
        cout << "Radi = " << radi << endl;
        dist = 3*radi;

        //cout << "Angle cam = " << ((atan(height()/(2*((3*dist) - dist))))*180)/3.1415 << endl;
        anglecam = ((asin(radi/dist)*180)/3.1415)*2;

        //anglecam = (((atan(radi/dist)*180)/3.1415)*2);
        cout << "Angle cam = " << anglecam << endl;
        ra = (float)width()/(float)height();
        angleX = 0;
        angleY = 0;
        angleZ = 0;
        anterior = radi;
        posterior = radi*3 + anterior;
        focus();

        //FI MOD

    GLfloat materialSpecular[] = {1.0f, 1.0f, 1.0f, 1.0f};
    GLfloat materialShininess[] = {50.0f};

    GLfloat lightPosition[] = {10.0f, 3.0f, 5.0f, 0.0f};
    GLfloat whiteLight[] = {1.0f, 1.0f, 1.0f, 1.0f};
    GLfloat modelAmbient[] = {0.1f, 0.1f, 0.1f, 1.0f};

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glShadeModel(GL_FLAT);

    glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);
    glMaterialfv(GL_FRONT, GL_SHININESS, materialShininess);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, whiteLight);
    glLightfv(GL_LIGHT0, GL_SPECULAR, whiteLight);

    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, modelAmbient);

    glEnable(GL_LIGHTING);
//	glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHT0);
    glEnable(GL_DEPTH_TEST);

    glPixelStorei(GL_UNPACK_ALIGNMENT,1);



    /*
    for(Models3DMap::iterator it=_models.begin(); it!=_models.end(); it++)
    {
        Model3D * model = (Model3D*)(it->second);
        model->registerTextures();
    }
    */
    //glEnable(GL_CULL_FACE);
    /*
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    glEnable(GL_LIGHTING);
    glEnable(GL_COLOR_MATERIAL);
*/
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
//	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    //Definim la posició de la nostra llum ambient.
    /*
    glLightfv(GL_LIGHT0, GL_POSITION, light_direction);
    //Definim la posició del SPOT.
    glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, spot_dir);
    */
}

//MOD
void DisplayVTK::focus()
{

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(anglecam, ra, anterior, posterior);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glTranslatef(0,0,-dist);
    glRotatef(-angleX,1,0,0);
    glRotatef(-angleY,0,1,0);
    glRotatef(-angleZ,0,0,1);
    glTranslatef(-VRP.x,-VRP.y,-VRP.z);
}
