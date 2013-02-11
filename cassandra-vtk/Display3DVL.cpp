#include <Display3DVL.h>
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

#include <vlCore/VisualizationLibrary.hpp>
#include <vlCore/Time.hpp>
#include <vlGraphics/Rendering.hpp>
#include <vlGraphics/SceneManagerActorTree.hpp>
#include <vlGraphics/Geometry.hpp>
#include <vlGraphics/GeometryPrimitives.hpp>
#include <vlGraphics/Light.hpp>
#include <vlGraphics/TrackballManipulator.hpp>
#include <vlGraphics/UIEventListener.hpp>
#include <vlGraphics/OpenGLContext.hpp>

#include <vlQt4/Qt4Widget.hpp>

using namespace vl;
using namespace vlQt4;


namespace GUI
{

//Inicialitzem els paràmetres al constructor per defecte i carreguem l'arxiu on hi ha tota la informació referent al ràster
//que posteriorment haurem de representar (x,y,z).
Display3DVL::Display3DVL(QWidget *parent ) : QGLWidget(parent), _simulationRecord(0), _viewedStep(0),  _cellScale(1.0f, 1.0f, 1.0f), _config3D(ProjectConfiguration::instance()->getConfig3D())
{

}

Display3DVL::~Display3DVL()
{
}


void Display3DVL::paintlandscape()
{


}


    void Display3DVL::initializeGL()
      {

        VisualizationLibrary::init();

        /* setup the OpenGL context format */
        OpenGLContextFormat format;
        format.setDoubleBuffer(true);
        format.setRGBABits( 8,8,8,8 );
        format.setDepthBufferBits(24);
        format.setStencilBufferBits(8);
        format.setFullscreen(false);
        //format.setMultisampleSamples(16);
        //format.setMultisample(true);

        ref<vl::Geometry> cube = vl::makeBox( vl::vec3(0,0,0), 10, 10, 10 );
        cube->computeNormals();
        // setup the effect to be used to render the cube
        ref<Effect> effect = new vl::Effect;
        // enable depth test and lighting
        effect->shader()->enable(vl::EN_DEPTH_TEST);
        // add a Light to the scene, since no Transform is associated to the Light it will follow the camera
        effect->shader()->setRenderState( new vl::Light, 0 );
        // enable the standard OpenGL lighting
        effect->shader()->enable(vl::EN_LIGHTING);
        // set the front and back material color of the cube
        // "gocMaterial" stands for "get-or-create Material"
        effect->shader()->gocMaterial()->setDiffuse( vl::crimson );

        /* create a native Qt4 window */
        ref<vlQt4::Qt4Widget> qt4_window = new vlQt4::Qt4Widget;

        /* define the camera position and orientation */
        vec3 eye    = vec3(0,10,35); // camera position
        vec3 center = vec3(0,0,0);   // point the camera is looking at
        vec3 up     = vec3(0,1,0);   // up direction
        /* Initialize the OpenGL context and window properties */
        int x = 10;
        int y = 10;
        int width = 512;
        int height= 512;
        qt4_window->initQt4Widget( "Visualization Library on Qt4 - Rotating Cube", format, NULL, x, y, width, height );
        /* show the window */
        qt4_window->show();

        /* deallocate the window with all the OpenGL resources before shutting down Visualization Library */
        qt4_window = NULL;

        /* shutdown Visualization Library */
        VisualizationLibrary::shutdown();

      }



void Display3DVL::updateGL()
{


}



void Display3DVL::setSimulationRecord( Engine::SimulationRecord * simulationRecord )
{
    _simulationRecord = simulationRecord;
    if(!_simulationRecord)
    {
        _orderedRasters.clear();
    }
}

}
