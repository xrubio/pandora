#include <Display3DVTK.h>
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

#include "vtkActor.h"
#include "vtkCamera.h"
#include "vtkCellArray.h"
#include "vtkFloatArray.h"
#include "vtkPointData.h"
#include "vtkPoints.h"
#include "vtkPolyData.h"
#include "vtkPolyDataMapper.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderer.h"
#include <vtkPolyDataReader.h>
#include <vtkTriangleFilter.h>
#include <vtkStripper.h>

namespace GUI
{

//Inicialitzem els paràmetres al constructor per defecte i carreguem l'arxiu on hi ha tota la informació referent al ràster
//que posteriorment haurem de representar (x,y,z).
Display3DVTK::Display3DVTK(QVTKWidget *parent) : QVTKWidget(parent), _simulationRecord(0), _viewedStep(0),  _cellScale(1.0f, 1.0f, 1.0f), _config3D(ProjectConfiguration::instance()->getConfig3D())
{

}

Display3DVTK::~Display3DVTK()
{
}

void Display3DVTK::paintLandscape()
{

    Engine::Point2D<int> index;
//  Engine::Raster & aster = _simulationRecord->getRaster(_selectedRaster, _viewedStep);
    Engine::StaticRaster & DEMRaster = _simulationRecord->getRasterTmp(_config3D.getDEMRaster(), _viewedStep);
    _landscapeMaterial.registerTexture();
    Engine::StaticRaster & colorRaster = _simulationRecord->getRasterTmp(_config3D.getColorRaster(), _viewedStep);

    _landscapeMaterial.activate();
    const Engine::Point3D<float> scale(_config3D.getSize3D());


//	glTranslatef(-_cellScale._x*_simulationRecord->getSize()/2, -_cellScale._y*_simulationRecord->getSize()/2, 0.0f);
    RasterConfiguration * rasterConfig = ProjectConfiguration::instance()->getRasterConfig(_config3D.getColorRaster());
    const ColorSelector & colorSelector(rasterConfig->getColorRamp());

    vtkPolyData *landscape = vtkPolyData::New();
    vtkPoints *points = vtkPoints::New();
    vtkCellArray *polys = vtkCellArray::New();
    vtkFloatArray *scalars = vtkFloatArray::New();

    int i = 0;
    float x[3];
    int y[4];
    for(index._x=0; index._x<DEMRaster.getSize()._x-1; index._x++)
    {
        for(index._y=0; index._y<DEMRaster.getSize()._y-1; index._y++)
        {
            Engine::Point3D<float> topLeft(index._x, index._y, 0.0f);
            topLeft._z = DEMRaster.getValue(Engine::Point2D<int>(topLeft._x,topLeft._y));
            float topLeftColor = colorRaster.getValue(Engine::Point2D<int>(topLeft._x,topLeft._y));

            Engine::Point3D<float> topRight(index._x+1, index._y, 0.0f);
            topRight._z = DEMRaster.getValue(Engine::Point2D<int>(topRight._x,topRight._y));
            float topRightColor = colorRaster.getValue(Engine::Point2D<int>(topRight._x,topRight._y));

            Engine::Point3D<float> bottomRight(index._x+1, index._y+1, 0.0f);
            bottomRight._z = DEMRaster.getValue(Engine::Point2D<int>(bottomRight._x,bottomRight._y));
            float bottomRightColor = colorRaster.getValue(Engine::Point2D<int>(bottomRight._x,bottomRight._y));

            Engine::Point3D<float> bottomLeft(index._x, index._y+1, 0.0f);
            bottomLeft._z = DEMRaster.getValue(Engine::Point2D<int>(bottomLeft._x,bottomLeft._y));
            float bottomLeftColor = colorRaster.getValue(Engine::Point2D<int>(bottomLeft._x,bottomLeft._y));

            // normal
            Engine::Point3D<float> a = bottomLeft;
            a = a - topRight;
            Engine::Point3D<float> b = bottomRight;
            b = b - topRight;

            Engine::Point3D<float> c = a.crossProduct(b);
            c = c.normalize();
            //glNormal3f(c._x, c._y, -c._z);

            x[0] = scale._x*topLeft._x;
            x[1] = -scale._y*topLeft._y;
            x[2] = scale._z*topLeft._z;
            points->InsertPoint(i,x);

            x[0] = scale._x*bottomLeft._x;
            x[1] = -scale._y*bottomLeft._y;
            x[2] = scale._z*bottomLeft._z;
            points->InsertPoint(i+1,x);

            x[0] = scale._x*bottomRight._x;
            x[1] = -scale._y*bottomRight._y;
            x[2] = scale._z*bottomRight._z;
            points->InsertPoint(i+2,x);

            x[0] = scale._x*topRight._x;
            x[1] = -scale._y*topRight._y;
            x[2] = scale._z*topRight._z;
            points->InsertPoint(i+3,x);

            y[0] = i;
            y[1] = i+1;
            y[2] = i+2;
            y[3] = i+3;
            scalars->InsertTuple1(i,topLeftColor);
            scalars->InsertTuple1(i+1,bottomLeftColor);
            scalars->InsertTuple1(i+2,bottomRightColor);
            scalars->InsertTuple1(i+3,topRightColor);

            polys->InsertNextCell(4,y);
            i = i+4;
        }
    }

    //_landscapeMaterial.deactivate();
    landscape->SetPoints(points);
    points->Delete();
    landscape->SetPolys(polys);
    polys->Delete();
    //Color
    landscape->GetPointData()->SetScalars(scalars);
    scalars->Delete();
/*
    vtkTriangleFilter *tris = vtkTriangleFilter::New();
    tris->SetInput(landscape);
    tris->GetOutput()->ReleaseDataFlagOn();

    vtkStripper *strip = vtkStripper::New();
    strip->SetInput(tris->GetOutput());
    //strip->GetOutput()->ReleaseDataFlagOn();*/

    // Now we'll look at it.
   /* vtkPolyDataMapper *cubeMapper = vtkPolyDataMapper::New();
    cubeMapper->SetInput(landscape);
    cubeMapper->SetScalarRange(0,i-1); //Colors
    cubeMapper->GlobalImmediateModeRenderingOn();*/

    // Now we'll look at it.
    vtkPolyDataMapper *mapper = vtkPolyDataMapper::New();
    mapper->SetInput(landscape);
    mapper->SetScalarRange(0.5,6.5);
    //mapper->ImmediateModeRenderingOn();



    vtkActor *cubeActor = vtkActor::New();
        cubeActor->SetMapper(mapper);


    // The usual rendering stuff.
    vtkCamera *camera = vtkCamera::New();
        camera->SetPosition(1,1,1);
        camera->SetFocalPoint(0,0,0);

    vtkRenderer *renderer = vtkRenderer::New();
    vtkRenderWindow *renWin = vtkRenderWindow::New();
      renWin->AddRenderer(renderer);

    vtkRenderWindowInteractor *iren = vtkRenderWindowInteractor::New();
        iren->SetRenderWindow(renWin);
    renderer->AddActor(cubeActor);
    renderer->SetActiveCamera(camera);
    renderer->ResetCamera();
    renderer->SetBackground(1,1,1);

    renWin->SetSize(300,300);

    // interact with data
    renWin->Render();
    iren->Start();

    // Clean up

    mapper->Delete();
    cubeActor->Delete();
    camera->Delete();
    renderer->Delete();
    renWin->Delete();
    iren->Delete();

}

void Display3DVTK::setSimulationRecord( Engine::SimulationRecord * simulationRecord )
{
    _simulationRecord = simulationRecord;
    if(!_simulationRecord)
    {
        _orderedRasters.clear();
    }
}

}
