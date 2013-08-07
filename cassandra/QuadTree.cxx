
#include "QuadTree.hxx"

#include <GL/gl.h>
#include <GL/glu.h>
#include <QListWidgetItem>
#include <ctime>
#include <cstdlib>

namespace GUI {

Quadtree::Quadtree(Engine::Point2D<int> center, Engine::Point2D<int> NW, Engine::Point2D<int> NE, Engine::Point2D<int> SE, Engine::Point2D<int> SW, Engine::Point2D<int> neighN, Engine::Point2D<int> neighS, Engine::Point2D<int> neighE,Engine::Point2D<int> neighW)
{
    this->NW = NW;
    this->NE = NE;
    this->SE = SE;
    this->SW = SW;
    this->center = center;
    this->neighN = neighN;
    this->neighS = neighS;
    this->neighE = neighE;
    this->neighW = neighW;
}

Quadtree*  Quadtree::getChildNodeNW()
{
    return this->childNW;
}

Quadtree*  Quadtree::getChildNodeNE()
{
    return this->childNE;

}
Quadtree*  Quadtree::getChildNodeSE()
{
    return this->childSE;

}
Quadtree*  Quadtree::getChildNodeSW()
{
    return this->childSW;
}

Engine::Point2D<int>  Quadtree::getOuterPointNW()
{
    return this->NW;
}

Engine::Point2D<int>  Quadtree::getOuterPointNE()
{
    return this->NE;
}

Engine::Point2D<int>  Quadtree::getOuterPointSE()
{
    return this->SE;
}

Engine::Point2D<int>  Quadtree::getOuterPointSW()
{
    return this->SW;
}

void Quadtree::setChildNodes(Quadtree *NW, Quadtree *NE, Quadtree *SE, Quadtree *SW)
{
    this->childNW = NW;
    this->childNE = NE;
    this->childSE = SE;
    this->childSW = SW;
}


void Quadtree::setOuterPoints(Engine::Point2D<int> NW, Engine::Point2D<int> NE, Engine::Point2D<int> SE, Engine::Point2D<int> SW)
{
    this->NW = NW;
    this->NE = NE;
    this->SE = SE;
    this->SW = SW;
}


void Quadtree::setCenter(Engine::Point2D<int>  center)
{
    this->center = center;

}


bool Quadtree::PolygonInFrustum(Engine::Point3D<int> a, Engine::Point3D<int> b, Engine::Point3D<int> c )
{
   int f, p;

   for( f = 0; f < 6; f++ )
   {
         p = 0;
         if( frustum[f][0] * a._x + frustum[f][1] * a._y + frustum[f][2] * a._z + frustum[f][3] < 0 ) p++;
         if( frustum[f][0] * b._x + frustum[f][1] * b._y + frustum[f][2] * b._z + frustum[f][3] < 0 ) p++;
         if( frustum[f][0] * c._x + frustum[f][1] * c._y + frustum[f][2] * c._z + frustum[f][3] < 0 ) p++;

      if( p == 3)
         return true;
   }
   return true;
}

void Quadtree::setFrustum(float frust[6][4])
{
    for (int i = 0; i < 6; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            frustum[i][j] = frust[i][j];
        }
    }
}

//NETEJAR DEMRaster (NO NECESARI, es carreguen els valors de Z quan es pinta), guardar nomes 2 punts a larbre (X,Y)
void Quadtree::initializeChilds( int size)
{
    int pot2 = powf(2,ceil(log2(size)));
    int prof = pot2/2;
    /*this->childNW = initializeChild(Engine::Point3D<int>(this->center._x/2,this->center._y/2,DEMRaster.getValue(Engine::Point2D<int>(this->center._x/2,this->center._y/2))),prof/2,DEMRaster);
    this->childNE = initializeChild(Engine::Point3D<int>(this->center._x+(this->center._x/2),this->center._y/2,DEMRaster.getValue(Engine::Point2D<int>(this->center._x+(this->center._x/2),this->center._y/2))),prof/2,DEMRaster);
    this->childSE = initializeChild(Engine::Point3D<int>(this->center._x+(this->center._x/2),this->center._y+(this->center._y/2),DEMRaster.getValue(Engine::Point2D<int>(this->center._x+(this->center._x/2),this->center._y+(this->center._y/2)))),prof/2,DEMRaster);
    this->childSW = initializeChild(Engine::Point3D<int>(this->center._x/2,this->center._y+(this->center._y/2),DEMRaster.getValue(Engine::Point2D<int>(this->center._x/2,this->center._y+(this->center._y/2)))),prof/2,DEMRaster);
    */
    prof = prof / 2;

    this->childNW = initializeChild(Engine::Point2D<int>(this->center._x-prof,this->center._y-prof),prof,size);

    this->childNE = initializeChild(Engine::Point2D<int>(this->center._x+prof,this->center._y-prof),prof,size);

    this->childSE = initializeChild(Engine::Point2D<int>(this->center._x+prof,this->center._y+prof),prof,size);

    this->childSW = initializeChild(Engine::Point2D<int>(this->center._x-prof,this->center._y+prof),prof,size);

}

Quadtree* Quadtree::initializeChild(Engine::Point2D<int> center, int prof, int size)
{
    if (prof >= 1)
    {
        //Engine::Point2D<int> NW, NE, SE, SW, neighN, neighS, neighE, neighW;
        Quadtree *child;

        Engine::Point2D<int> NW(center._x-prof,center._y-prof);
        Engine::Point2D<int> NE(center._x+prof,center._y-prof);
        Engine::Point2D<int> SE(center._x+prof,center._y+prof);
        Engine::Point2D<int> SW(center._x-prof,center._y+prof);

        Engine::Point2D<int> neighN(center._x,center._y-prof);
        Engine::Point2D<int> neighS(center._x,center._y+prof);
        Engine::Point2D<int> neighE(center._x+prof,center._y);
        Engine::Point2D<int> neighW(center._x-prof,center._y);

        child = new Quadtree(center,NW,NE,SE,SW,neighN,neighS,neighE,neighW);
        prof=prof/2;

        child->childNW = initializeChild(Engine::Point2D<int>(center._x-prof,center._y-prof),prof,size);
        child->childNE = initializeChild(Engine::Point2D<int>(center._x+prof,center._y-prof),prof,size);
        child->childSE = initializeChild(Engine::Point2D<int>(center._x+prof,center._y+prof),prof,size);
        child->childSW = initializeChild(Engine::Point2D<int>(center._x-prof,center._y+prof),prof,size);

        return child;
    }
    else return 0;

}

int Quadtree::update(double dist, int prof, ColorSelector &colorSelector, Engine::StaticRaster & colorRaster, int size, const Engine::Point3D<float> scale, Engine::StaticRaster & DEMRaster, int LOD, int offset)
{
    //si la distancia(camara-punt(anglecam))/profunditat_arbre < Constant -> habilita punt
    //cout << "Relacio " << (abs(dist-this->center._z))/prof << endl;
    //MIRAR POSICIO CAMARA, NO DIST!!!

    float camara1[16];
    glGetFloatv(GL_MODELVIEW_MATRIX,camara1);
    //cout << camara1[12] << " " << camara1[13] << " " <<camara1[14] << endl;

    /*Engine::Point3D<float> camara(camara1[0]*camara1[12]+camara1[4]*camara1[13]+camara1[8]*camara1[14],
                                  camara1[1]*camara1[12]+camara1[5]*camara1[13]+camara1[9]*camara1[14],
                                  camara1[2]*camara1[12]+camara1[6]*camara1[13]+camara1[10]*camara1[14]);*/

    Engine::Point3D<float> camara(0,0,0);


    float px, py, pz;

    if (center._x < DEMRaster.getSize()._x && center._y < DEMRaster.getSize()._y) {
        px = camara1[0] * center._x + -camara1[4] * center._y + camara1[8] * DEMRaster.getValue(Engine::Point2D<int>(center._x,center._y)) +camara1[12];
        py = camara1[1] * center._x + -camara1[5] * center._y + camara1[9] * DEMRaster.getValue(Engine::Point2D<int>(center._x,center._y)) +camara1[13];
        pz = camara1[2] * center._x + -camara1[6] * center._y + camara1[10]* DEMRaster.getValue(Engine::Point2D<int>(center._x,center._y)) +camara1[14];
    }
    /*else {
        px = camara1[0] * center._x + -camara1[4] * center._y + camara1[8]  +camara1[12];
        py = camara1[1] * center._x + -camara1[5] * center._y + camara1[9]  +camara1[13];
        pz = camara1[2] * center._x + -camara1[6] * center._y + camara1[10] +camara1[14];
    }*/

    Engine::Point3D<float> punt(px,py,pz);

    //cout << camara.distance(punt)/prof << " " << prof << endl;

    if (abs(camara.distance(punt))/prof < LOD and this->childNW != 0)
    {
        int profNW = prof;
        int aux;
        if (this->childNW != 0)
        {
            aux = this->childNW->update(dist,prof/2,colorSelector,colorRaster,size,scale,DEMRaster,LOD,offset);
            if (aux < prof) profNW = aux;
        }

        int profNE = prof;
        if (childNE != 0)
        {
            aux = this->childNE->update(dist,prof/2,colorSelector,colorRaster,size,scale,DEMRaster,LOD,offset);
            if (aux < prof) profNE = aux;
        }

        int profSE = prof;
        if (childSE != 0)
        {
            aux = this->childSE->update(dist,prof/2,colorSelector,colorRaster,size,scale,DEMRaster,LOD,offset);
            if (aux < prof) profSE = aux;
        }

        int profSW = prof;
        if (childSW != 0)
        {
            aux = this->childSW->update(dist,prof/2,colorSelector,colorRaster,size,scale,DEMRaster,LOD,offset);
            if (aux < prof) profSW = aux;
        }
/*
        float color;
        //CRACKS
        if (profNW < profNE and childNW != 0 and childNE != 0)
        {
            //NW(NE,E),NE(center)
            glBegin(GL_TRIANGLES);
            color = colorRaster.getValue(Engine::Point2D<int>(childNE->center._x,childNE->center._y));
            setCellColor(colorSelector.getColor(color));
            glTexCoord2f(childNE->center._x/size, childNE->center._y/size);
           glVertex3f(childNE->center._x*scale._x,-childNE->center._y*scale._y,DEMRaster.getValue(Engine::Point2D<int>(childNE->center._x,childNE->center._y))*scale._z+offset);
            //glVertex3f(center._x*scale._x,-center._y*scale._y,1);

            color = colorRaster.getValue(Engine::Point2D<int>(childNW->NE._x,childNW->NE._y));
            setCellColor(colorSelector.getColor(color));
            glTexCoord2f(childNW->NE._x/size, childNW->NE._y/size);
            glVertex3f(childNW->NE._x*scale._x,-childNW->NE._y*scale._y,DEMRaster.getValue(Engine::Point2D<int>(childNW->NE._x,childNW->NE._y))*scale._z+offset);
           //glVertex3f(neighW._x*scale._x,-neighW._y*scale._y,1);

            color = colorRaster.getValue(Engine::Point2D<int>(childNW->neighE._x,childNW->neighE._y));
            setCellColor(colorSelector.getColor(color));
            glTexCoord2f(childNW->neighE._x/size, childNW->neighE._y/size);
            glVertex3f(childNW->neighE._x*scale._x,-childNW->neighE._y*scale._y,DEMRaster.getValue(Engine::Point2D<int>(childNW->neighE._x,childNW->neighE._y))*scale._z+offset);
           //glVertex3f(NW._x*scale._x,-NW._y*scale._y,1);
            glEnd();

            //NW(E,SE),NE(center)
            glBegin(GL_TRIANGLES);
            color = colorRaster.getValue(Engine::Point2D<int>(childNE->center._x,childNE->center._y));
            setCellColor(colorSelector.getColor(color));
            glTexCoord2f(childNE->center._x/size, childNE->center._y/size);
           glVertex3f(childNE->center._x*scale._x,-childNE->center._y*scale._y,DEMRaster.getValue(Engine::Point2D<int>(childNE->center._x,childNE->center._y))*scale._z+offset);
            //glVertex3f(center._x*scale._x,-center._y*scale._y,1);

           color = colorRaster.getValue(Engine::Point2D<int>(childNW->neighE._x,childNW->neighE._y));
           setCellColor(colorSelector.getColor(color));
           glTexCoord2f(childNW->neighE._x/size, childNW->neighE._y/size);
           glVertex3f(childNW->neighE._x*scale._x,-childNW->neighE._y*scale._y,DEMRaster.getValue(Engine::Point2D<int>(childNW->neighE._x,childNW->neighE._y))*scale._z+offset);
          //glVertex3f(NW._x*scale._x,-NW._y*scale._y,1);

            color = colorRaster.getValue(Engine::Point2D<int>(childNW->SE._x,childNW->SE._y));
            setCellColor(colorSelector.getColor(color));
            glTexCoord2f(childNW->SE._x/size, childNW->SE._y/size);
            glVertex3f(childNW->SE._x*scale._x,-childNW->SE._y*scale._y,DEMRaster.getValue(Engine::Point2D<int>(childNW->SE._x,childNW->SE._y))*scale._z+offset);
           //glVertex3f(neighW._x*scale._x,-neighW._y*scale._y,1);
            glEnd();
        }

        if (profNW < profSW and childNW != 0 and childSW != 0)
        {
            //NW(SE,S),SW(center)
            glBegin(GL_TRIANGLES);
            color = colorRaster.getValue(Engine::Point2D<int>(childSW->center._x,childSW->center._y));
            setCellColor(colorSelector.getColor(color));
            glTexCoord2f(childSW->center._x/size, childSW->center._y/size);
           glVertex3f(childSW->center._x*scale._x,-childSW->center._y*scale._y,DEMRaster.getValue(Engine::Point2D<int>(childSW->center._x,childSW->center._y))*scale._z+offset);
            //glVertex3f(center._x*scale._x,-center._y*scale._y,1);

            color = colorRaster.getValue(Engine::Point2D<int>(childNW->SE._x,childNW->SE._y));
            setCellColor(colorSelector.getColor(color));
            glTexCoord2f(childNW->SE._x/size, childNW->SE._y/size);
            glVertex3f(childNW->SE._x*scale._x,-childNW->SE._y*scale._y,DEMRaster.getValue(Engine::Point2D<int>(childNW->SE._x,childNW->SE._y))*scale._z+offset);
           //glVertex3f(neighW._x*scale._x,-neighW._y*scale._y,1);

            color = colorRaster.getValue(Engine::Point2D<int>(childNW->neighS._x,childNW->neighS._y));
            setCellColor(colorSelector.getColor(color));
            glTexCoord2f(childNW->neighS._x/size, childNW->neighS._y/size);
            glVertex3f(childNW->neighS._x*scale._x,-childNW->neighS._y*scale._y,DEMRaster.getValue(Engine::Point2D<int>(childNW->neighS._x,childNW->neighS._y))*scale._z+offset);
           //glVertex3f(NW._x*scale._x,-NW._y*scale._y,1);
            glEnd();

            //NW(S,SW),SW(center)
            glBegin(GL_TRIANGLES);
            color = colorRaster.getValue(Engine::Point2D<int>(childSW->center._x,childSW->center._y));
            setCellColor(colorSelector.getColor(color));
            glTexCoord2f(childSW->center._x/size, childSW->center._y/size);
           glVertex3f(childSW->center._x*scale._x,-childSW->center._y*scale._y,DEMRaster.getValue(Engine::Point2D<int>(childSW->center._x,childSW->center._y))*scale._z+offset);
            //glVertex3f(center._x*scale._x,-center._y*scale._y,1);

           color = colorRaster.getValue(Engine::Point2D<int>(childNW->neighS._x,childNW->neighS._y));
           setCellColor(colorSelector.getColor(color));
           glTexCoord2f(childNW->neighS._x/size, childNW->neighS._y/size);
           glVertex3f(childNW->neighS._x*scale._x,-childNW->neighS._y*scale._y,DEMRaster.getValue(Engine::Point2D<int>(childNW->neighS._x,childNW->neighS._y))*scale._z+offset);
          //glVertex3f(NW._x*scale._x,-NW._y*scale._y,1);

            color = colorRaster.getValue(Engine::Point2D<int>(childNW->SW._x,childNW->SW._y));
            setCellColor(colorSelector.getColor(color));
            glTexCoord2f(childNW->SW._x/size, childNW->SW._y/size);
            glVertex3f(childNW->SW._x*scale._x,-childNW->SW._y*scale._y,DEMRaster.getValue(Engine::Point2D<int>(childNW->SW._x,childNW->SW._y))*scale._z+offset);
           //glVertex3f(neighW._x*scale._x,-neighW._y*scale._y,1);
            glEnd();
        }

        if (profNE < profSE and childNE != 0 and childSE != 0)
        {
            //NE(SW,S),SE(center)
            glBegin(GL_TRIANGLES);
            color = colorRaster.getValue(Engine::Point2D<int>(childSE->center._x,childSE->center._y));
            setCellColor(colorSelector.getColor(color));
            glTexCoord2f(childSE->center._x/size, childSE->center._y/size);
           glVertex3f(childSE->center._x*scale._x,-childSE->center._y*scale._y,DEMRaster.getValue(Engine::Point2D<int>(childSE->center._x,childSE->center._y))*scale._z+offset);
            //glVertex3f(center._x*scale._x,-center._y*scale._y,1);

           color = colorRaster.getValue(Engine::Point2D<int>(childNE->neighS._x,childNE->neighS._y));
           setCellColor(colorSelector.getColor(color));
           glTexCoord2f(childNE->neighS._x/size, childNE->neighS._y/size);
           glVertex3f(childNE->neighS._x*scale._x,-childNE->neighS._y*scale._y,DEMRaster.getValue(Engine::Point2D<int>(childNE->neighS._x,childNE->neighS._y))*scale._z+offset);
          //glVertex3f(NW._x*scale._x,-NW._y*scale._y,1);

            color = colorRaster.getValue(Engine::Point2D<int>(childNE->SW._x,childNE->SW._y));
            setCellColor(colorSelector.getColor(color));
            glTexCoord2f(childNE->SW._x/size, childNE->SW._y/size);
            glVertex3f(childNE->SW._x*scale._x,-childNE->SW._y*scale._y,DEMRaster.getValue(Engine::Point2D<int>(childNE->SW._x,childNE->SW._y))*scale._z+offset);
           //glVertex3f(neighW._x*scale._x,-neighW._y*scale._y,1);
            glEnd();

            //NE(S,SE),SE(center)
            glBegin(GL_TRIANGLES);
            color = colorRaster.getValue(Engine::Point2D<int>(childSE->center._x,childSE->center._y));
            setCellColor(colorSelector.getColor(color));
            glTexCoord2f(childSE->center._x/size, childSE->center._y/size);
           glVertex3f(childSE->center._x*scale._x,-childSE->center._y*scale._y,DEMRaster.getValue(Engine::Point2D<int>(childSE->center._x,childSE->center._y))*scale._z+offset);
            //glVertex3f(center._x*scale._x,-center._y*scale._y,1);

            color = colorRaster.getValue(Engine::Point2D<int>(childNE->SE._x,childNE->SE._y));
            setCellColor(colorSelector.getColor(color));
            glTexCoord2f(childNE->SE._x/size, childNE->SE._y/size);
            glVertex3f(childNE->SE._x*scale._x,-childNE->SE._y*scale._y,DEMRaster.getValue(Engine::Point2D<int>(childNE->SE._x,childNE->SE._y))*scale._z+offset);
           //glVertex3f(neighW._x*scale._x,-neighW._y*scale._y,1);

            color = colorRaster.getValue(Engine::Point2D<int>(childNE->neighS._x,childNE->neighS._y));
            setCellColor(colorSelector.getColor(color));
            glTexCoord2f(childNE->neighS._x/size, childNE->neighS._y/size);
            glVertex3f(childNE->neighS._x*scale._x,-childNE->neighS._y*scale._y,DEMRaster.getValue(Engine::Point2D<int>(childNE->neighS._x,childNE->neighS._y))*scale._z+offset);
           //glVertex3f(NE._x*scale._x,-NE._y*scale._y,1);
            glEnd();
        }

        if (profNE < profNW and childNE != 0 and childNW != 0)
        {
            //NE(W,NW),NW(center)
            glBegin(GL_TRIANGLES);
            color = colorRaster.getValue(Engine::Point2D<int>(childNW->center._x,childNW->center._y));
            setCellColor(colorSelector.getColor(color));
            glTexCoord2f(childNW->center._x/size, childNW->center._y/size);
           glVertex3f(childNW->center._x*scale._x,-childNW->center._y*scale._y,DEMRaster.getValue(Engine::Point2D<int>(childNW->center._x,childNW->center._y))*scale._z+offset);
            //glVertex3f(center._x*scale._x,-center._y*scale._y,1);

           color = colorRaster.getValue(Engine::Point2D<int>(childNE->neighW._x,childNE->neighW._y));
           setCellColor(colorSelector.getColor(color));
           glTexCoord2f(childNE->neighW._x/size, childNE->neighW._y/size);
           glVertex3f(childNE->neighW._x*scale._x,-childNE->neighW._y*scale._y,DEMRaster.getValue(Engine::Point2D<int>(childNE->neighW._x,childNE->neighW._y))*scale._z+offset);
          //glVertex3f(NW._x*scale._x,-NW._y*scale._y,1);

            color = colorRaster.getValue(Engine::Point2D<int>(childNE->NW._x,childNE->NW._y));
            setCellColor(colorSelector.getColor(color));
            glTexCoord2f(childNE->NW._x/size, childNE->NW._y/size);
            glVertex3f(childNE->NW._x*scale._x,-childNE->NW._y*scale._y,DEMRaster.getValue(Engine::Point2D<int>(childNE->NW._x,childNE->NW._y))*scale._z+offset);
           //glVertex3f(neighW._x*scale._x,-neighW._y*scale._y,1);
            glEnd();

            //NE(SW,W),NW(center)
            glBegin(GL_TRIANGLES);
            color = colorRaster.getValue(Engine::Point2D<int>(childNW->center._x,childNW->center._y));
            setCellColor(colorSelector.getColor(color));
            glTexCoord2f(childNW->center._x/size, childNW->center._y/size);
           glVertex3f(childNW->center._x*scale._x,-childNW->center._y*scale._y,DEMRaster.getValue(Engine::Point2D<int>(childNW->center._x,childNW->center._y))*scale._z+offset);
            //glVertex3f(center._x*scale._x,-center._y*scale._y,1);

            color = colorRaster.getValue(Engine::Point2D<int>(childNE->SW._x,childNE->SW._y));
            setCellColor(colorSelector.getColor(color));
            glTexCoord2f(childNE->SW._x/size, childNE->SW._y/size);
            glVertex3f(childNE->SW._x*scale._x,-childNE->SW._y*scale._y,DEMRaster.getValue(Engine::Point2D<int>(childNE->SW._x,childNE->SW._y))*scale._z+offset);
           //glVertex3f(neighW._x*scale._x,-neighW._y*scale._y,1);

            color = colorRaster.getValue(Engine::Point2D<int>(childNE->neighW._x,childNE->neighW._y));
            setCellColor(colorSelector.getColor(color));
            glTexCoord2f(childNE->neighW._x/size, childNE->neighW._y/size);
            glVertex3f(childNE->neighW._x*scale._x,-childNE->neighW._y*scale._y,DEMRaster.getValue(Engine::Point2D<int>(childNE->neighW._x,childNE->neighW._y))*scale._z+offset);
           //glVertex3f(NE._x*scale._x,-NE._y*scale._y,1);
            glEnd();
        }

        if (profSE < profSW and childSE != 0 and childSW != 0)
        {
            //SE(W,NW),SW(center)
            glBegin(GL_TRIANGLES);
            color = colorRaster.getValue(Engine::Point2D<int>(childSW->center._x,childSW->center._y));
            setCellColor(colorSelector.getColor(color));
            glTexCoord2f(childSW->center._x/size, childSW->center._y/size);
           glVertex3f(childSW->center._x*scale._x,-childSW->center._y*scale._y,DEMRaster.getValue(Engine::Point2D<int>(childSW->center._x,childSW->center._y))*scale._z+offset);
            //glVertex3f(center._x*scale._x,-center._y*scale._y,1);

           color = colorRaster.getValue(Engine::Point2D<int>(childSE->neighW._x,childSE->neighW._y));
           setCellColor(colorSelector.getColor(color));
           glTexCoord2f(childSE->neighW._x/size, childSE->neighW._y/size);
           glVertex3f(childSE->neighW._x*scale._x,-childSE->neighW._y*scale._y,DEMRaster.getValue(Engine::Point2D<int>(childSE->neighW._x,childSE->neighW._y))*scale._z+offset);
          //glVertex3f(NW._x*scale._x,-NW._y*scale._y,1);

            color = colorRaster.getValue(Engine::Point2D<int>(childSE->NW._x,childSE->NW._y));
            setCellColor(colorSelector.getColor(color));
            glTexCoord2f(childSE->NW._x/size, childSE->NW._y/size);
            glVertex3f(childSE->NW._x*scale._x,-childSE->NW._y*scale._y,DEMRaster.getValue(Engine::Point2D<int>(childSE->NW._x,childSE->NW._y))*scale._z+offset);
           //glVertex3f(neighW._x*scale._x,-neighW._y*scale._y,1);
            glEnd();

            //SE(SW,W),SW(center)
            glBegin(GL_TRIANGLES);
            color = colorRaster.getValue(Engine::Point2D<int>(childSW->center._x,childSW->center._y));
            setCellColor(colorSelector.getColor(color));
            glTexCoord2f(childSW->center._x/size, childSW->center._y/size);
           glVertex3f(childSW->center._x*scale._x,-childSW->center._y*scale._y,DEMRaster.getValue(Engine::Point2D<int>(childSW->center._x,childSW->center._y))*scale._z+offset);
            //glVertex3f(center._x*scale._x,-center._y*scale._y,1);

            color = colorRaster.getValue(Engine::Point2D<int>(childSE->SW._x,childSE->SW._y));
            setCellColor(colorSelector.getColor(color));
            glTexCoord2f(childSE->SW._x/size, childSE->SW._y/size);
            glVertex3f(childSE->SW._x*scale._x,-childSE->SW._y*scale._y,DEMRaster.getValue(Engine::Point2D<int>(childSE->SW._x,childSE->SW._y))*scale._z+offset);
           //glVertex3f(neighW._x*scale._x,-neighW._y*scale._y,1);

            color = colorRaster.getValue(Engine::Point2D<int>(childSE->neighW._x,childSE->neighW._y));
            setCellColor(colorSelector.getColor(color));
            glTexCoord2f(childSE->neighW._x/size, childSE->neighW._y/size);
            glVertex3f(childSE->neighW._x*scale._x,-childSE->neighW._y*scale._y,DEMRaster.getValue(Engine::Point2D<int>(childSE->neighW._x,childSE->neighW._y))*scale._z+offset);
           //glVertex3f(NE._x*scale._x,-NE._y*scale._y,1);
            glEnd();
        }

        if (profSE < profNE and childSE != 0 and childNE != 0)
        {
            //SE(N,NW),NE(center)
            glBegin(GL_TRIANGLES);
            color = colorRaster.getValue(Engine::Point2D<int>(childNE->center._x,childNE->center._y));
            setCellColor(colorSelector.getColor(color));
            glTexCoord2f(childNE->center._x/size, childNE->center._y/size);
           glVertex3f(childNE->center._x*scale._x,-childNE->center._y*scale._y,DEMRaster.getValue(Engine::Point2D<int>(childNE->center._x,childNE->center._y))*scale._z+offset);
            //glVertex3f(center._x*scale._x,-center._y*scale._y,1);

            color = colorRaster.getValue(Engine::Point2D<int>(childSE->NW._x,childSE->NW._y));
            setCellColor(colorSelector.getColor(color));
            glTexCoord2f(childSE->NW._x/size, childSE->NW._y/size);
            glVertex3f(childSE->NW._x*scale._x,-childSE->NW._y*scale._y,DEMRaster.getValue(Engine::Point2D<int>(childSE->NW._x,childSE->NW._y))*scale._z+offset);
           //glVertex3f(neighW._x*scale._x,-neighW._y*scale._y,1);

            color = colorRaster.getValue(Engine::Point2D<int>(childSE->neighN._x,childSE->neighN._y));
            setCellColor(colorSelector.getColor(color));
            glTexCoord2f(childSE->neighN._x/size, childSE->neighN._y/size);
            glVertex3f(childSE->neighN._x*scale._x,-childSE->neighN._y*scale._y,DEMRaster.getValue(Engine::Point2D<int>(childSE->neighN._x,childSE->neighN._y))*scale._z+offset);
           //glVertex3f(NW._x*scale._x,-NW._y*scale._y,1);
            glEnd();

            //SE(NE,N),NE(center)
            glBegin(GL_TRIANGLES);
            color = colorRaster.getValue(Engine::Point2D<int>(childNE->center._x,childNE->center._y));
            setCellColor(colorSelector.getColor(color));
            glTexCoord2f(childNE->center._x/size, childNE->center._y/size);
           glVertex3f(childNE->center._x*scale._x,-childNE->center._y*scale._y,DEMRaster.getValue(Engine::Point2D<int>(childNE->center._x,childNE->center._y))*scale._z+offset);
            //glVertex3f(center._x*scale._x,-center._y*scale._y,1);

           color = colorRaster.getValue(Engine::Point2D<int>(childSE->neighN._x,childSE->neighN._y));
           setCellColor(colorSelector.getColor(color));
           glTexCoord2f(childSE->neighN._x/size, childSE->neighN._y/size);
           glVertex3f(childSE->neighN._x*scale._x,-childSE->neighN._y*scale._y,DEMRaster.getValue(Engine::Point2D<int>(childSE->neighN._x,childSE->neighN._y))*scale._z+offset);
          //glVertex3f(NE._x*scale._x,-NE._y*scale._y,1);

            color = colorRaster.getValue(Engine::Point2D<int>(childSE->NE._x,childSE->NE._y));
            setCellColor(colorSelector.getColor(color));
            glTexCoord2f(childSE->NE._x/size, childSE->NE._y/size);
            glVertex3f(childSE->NE._x*scale._x,-childSE->NE._y*scale._y,DEMRaster.getValue(Engine::Point2D<int>(childSE->NE._x,childSE->NE._y))*scale._z+offset);
           //glVertex3f(neighW._x*scale._x,-neighW._y*scale._y,1);
            glEnd();
        }

        if (profSW < profSE and childSW != 0 and childSE != 0)
        {
            //SW(E,NE),SE(center)
            glBegin(GL_TRIANGLES);
            color = colorRaster.getValue(Engine::Point2D<int>(childSE->center._x,childSE->center._y));
            setCellColor(colorSelector.getColor(color));
            glTexCoord2f(childSE->center._x/size, childSE->center._y/size);
           glVertex3f(childSE->center._x*scale._x,-childSE->center._y*scale._y,DEMRaster.getValue(Engine::Point2D<int>(childSE->center._x,childSE->center._y))*scale._z+offset);
            //glVertex3f(center._x*scale._x,-center._y*scale._y,1);

            color = colorRaster.getValue(Engine::Point2D<int>(childSW->NE._x,childSW->NE._y));
            setCellColor(colorSelector.getColor(color));
            glTexCoord2f(childSW->NE._x/size, childSW->NE._y/size);
            glVertex3f(childSW->NE._x*scale._x,-childSW->NE._y*scale._y,DEMRaster.getValue(Engine::Point2D<int>(childSW->NE._x,childSW->NE._y))*scale._z+offset);
           //glVertex3f(neighW._x*scale._x,-neighW._y*scale._y,1);

            color = colorRaster.getValue(Engine::Point2D<int>(childSW->neighE._x,childSW->neighE._y));
            setCellColor(colorSelector.getColor(color));
            glTexCoord2f(childSW->neighE._x/size, childSW->neighE._y/size);
            glVertex3f(childSW->neighE._x*scale._x,-childSW->neighE._y*scale._y,DEMRaster.getValue(Engine::Point2D<int>(childSW->neighE._x,childSW->neighE._y))*scale._z+offset);
           //glVertex3f(NW._x*scale._x,-NW._y*scale._y,1);
            glEnd();

            //SW(SE,E),SE(center)
            glBegin(GL_TRIANGLES);
            color = colorRaster.getValue(Engine::Point2D<int>(childSE->center._x,childSE->center._y));
            setCellColor(colorSelector.getColor(color));
            glTexCoord2f(childSE->center._x/size, childSE->center._y/size);
           glVertex3f(childSE->center._x*scale._x,-childSE->center._y*scale._y,DEMRaster.getValue(Engine::Point2D<int>(childSE->center._x,childSE->center._y))*scale._z+offset);
            //glVertex3f(center._x*scale._x,-center._y*scale._y,1);

           color = colorRaster.getValue(Engine::Point2D<int>(childSW->neighE._x,childSW->neighE._y));
           setCellColor(colorSelector.getColor(color));
           glTexCoord2f(childSW->neighE._x/size, childSW->neighE._y/size);
           glVertex3f(childSW->neighE._x*scale._x,-childSW->neighE._y*scale._y,DEMRaster.getValue(Engine::Point2D<int>(childSW->neighE._x,childSW->neighE._y))*scale._z+offset);
          //glVertex3f(NE._x*scale._x,-NE._y*scale._y,1);

            color = colorRaster.getValue(Engine::Point2D<int>(childSW->SE._x,childSW->SE._y));
            setCellColor(colorSelector.getColor(color));
            glTexCoord2f(childSW->SE._x/size, childSW->SE._y/size);
            glVertex3f(childSW->SE._x*scale._x,-childSW->SE._y*scale._y,DEMRaster.getValue(Engine::Point2D<int>(childSW->SE._x,childSW->SE._y))*scale._z+offset);
           //glVertex3f(SEighW._x*scale._x,-SEighW._y*scale._y,1);
            glEnd();
        }

        if (profSW < profNW and childSW != 0 and childNW != 0)
        {
            //SW(N,NE),NW(center)
            glBegin(GL_TRIANGLES);
            color = colorRaster.getValue(Engine::Point2D<int>(childNW->center._x,childNW->center._y));
            setCellColor(colorSelector.getColor(color));
            glTexCoord2f(childNW->center._x/size, childNW->center._y/size);
           glVertex3f(childNW->center._x*scale._x,-childNW->center._y*scale._y,DEMRaster.getValue(Engine::Point2D<int>(childNW->center._x,childNW->center._y))*scale._z+offset);
            //glVertex3f(center._x*scale._x,-center._y*scale._y,1);

           color = colorRaster.getValue(Engine::Point2D<int>(childSW->neighN._x,childSW->neighN._y));
           setCellColor(colorSelector.getColor(color));
           glTexCoord2f(childSW->neighN._x/size, childSW->neighN._y/size);
           glVertex3f(childSW->neighN._x*scale._x,-childSW->neighN._y*scale._y,DEMRaster.getValue(Engine::Point2D<int>(childSW->neighN._x,childSW->neighN._y))*scale._z+offset);
          //glVertex3f(NW._x*scale._x,-NW._y*scale._y,1);

            color = colorRaster.getValue(Engine::Point2D<int>(childSW->NE._x,childSW->NE._y));
            setCellColor(colorSelector.getColor(color));
            glTexCoord2f(childSW->NE._x/size, childSW->NE._y/size);
            glVertex3f(childSW->NE._x*scale._x,-childSW->NE._y*scale._y,DEMRaster.getValue(Engine::Point2D<int>(childSW->NE._x,childSW->NE._y))*scale._z+offset);
           //glVertex3f(neighW._x*scale._x,-neighW._y*scale._y,1);
            glEnd();

            //SW(NW,N),NW(center)
            glBegin(GL_TRIANGLES);
            color = colorRaster.getValue(Engine::Point2D<int>(childNW->center._x,childNW->center._y));
            setCellColor(colorSelector.getColor(color));
            glTexCoord2f(childNW->center._x/size, childNW->center._y/size);
           glVertex3f(childNW->center._x*scale._x,-childNW->center._y*scale._y,DEMRaster.getValue(Engine::Point2D<int>(childNW->center._x,childNW->center._y))*scale._z+offset);
            //glVertex3f(center._x*scale._x,-center._y*scale._y,1);

            color = colorRaster.getValue(Engine::Point2D<int>(childSW->NW._x,childSW->NW._y));
            setCellColor(colorSelector.getColor(color));
            glTexCoord2f(childSW->NW._x/size, childSW->NW._y/size);
            glVertex3f(childSW->NW._x*scale._x,-childSW->NW._y*scale._y,DEMRaster.getValue(Engine::Point2D<int>(childSW->NW._x,childSW->NW._y))*scale._z+offset);
           //glVertex3f(SEighW._x*scale._x,-SEighW._y*scale._y,1);

            color = colorRaster.getValue(Engine::Point2D<int>(childSW->neighN._x,childSW->neighN._y));
            setCellColor(colorSelector.getColor(color));
            glTexCoord2f(childSW->neighN._x/size, childSW->neighN._y/size);
            glVertex3f(childSW->neighN._x*scale._x,-childSW->neighN._y*scale._y,DEMRaster.getValue(Engine::Point2D<int>(childSW->neighN._x,childSW->neighN._y))*scale._z+offset);
           //glVertex3f(NE._x*scale._x,-NE._y*scale._y,1);
            glEnd();
        }

        int min = profNW;
        if (profNE < min) min = profNE;
        if (profSE < min) min = profSE;
        if (profSW < min) min = profSW;
        return min;
        */

    }
    else
    {
        float color;


        //glBegin(GL_LINES);
            //C-W-NW
            if (center._x < DEMRaster.getSize()._x && center._y < DEMRaster.getSize()._x) {

            if (neighW._x < DEMRaster.getSize()._x && neighW._y < DEMRaster.getSize()._x && NW._x < DEMRaster.getSize()._x && NW._y < DEMRaster.getSize()._x
                      && PolygonInFrustum(Engine::Point3D<int>(neighW._x,neighW._y,DEMRaster.getValue(Engine::Point2D<int>(neighW._x,neighW._y))), Engine::Point3D<int>(NW._x,NW._y,DEMRaster.getValue(Engine::Point2D<int>(NW._x,NW._y))), Engine::Point3D<int>(center._x,center._y,DEMRaster.getValue(Engine::Point2D<int>(center._x,center._y)))))
            {
                glBegin(GL_TRIANGLES);

                if(DEMRaster.hasColorTable())
                {
                    Engine::ColorEntry color = DEMRaster.getColorEntry(DEMRaster.getValue(Engine::Point2D<int>(center._x,center._y)));
                    setCellColor(QColor(color._r, color._g, color._b, color._alpha));
                }
                else {
                    color = colorRaster.getValue(Engine::Point2D<int>(center._x,center._y));
                    setCellColor(colorSelector.getColor(color));
                }
                glTexCoord2f(center._x/size, center._y/size);
                glVertex3f(center._x*scale._x,-center._y*scale._y,(DEMRaster.getValue(Engine::Point2D<int>(center._x,center._y))+offset));
                //glVertex3f(center._x*scale._x,-center._y*scale._y,1);

                if(DEMRaster.hasColorTable())
                {
                    Engine::ColorEntry color = DEMRaster.getColorEntry(DEMRaster.getValue(Engine::Point2D<int>(NW._x,NW._y)));
                    setCellColor(QColor(color._r, color._g, color._b, color._alpha));
                }
                else {
                    color = colorRaster.getValue(Engine::Point2D<int>(NW._x,NW._y));
                    setCellColor(colorSelector.getColor(color));
                }
                glTexCoord2f(NW._x/size, NW._y/size);
                glVertex3f(NW._x*scale._x,-NW._y*scale._y,(DEMRaster.getValue(Engine::Point2D<int>(NW._x,NW._y))+offset));
               //glVertex3f(NW._x*scale._x,-NW._y*scale._y,1);

                if(DEMRaster.hasColorTable())
                {
                    Engine::ColorEntry color = DEMRaster.getColorEntry(DEMRaster.getValue(Engine::Point2D<int>(neighW._x,neighW._y)));
                    setCellColor(QColor(color._r, color._g, color._b, color._alpha));
                }
                else {
                    color = colorRaster.getValue(Engine::Point2D<int>(neighW._x,neighW._y));
                    setCellColor(colorSelector.getColor(color));
                }
                glTexCoord2f(neighW._x/size, neighW._y/size);
                glVertex3f(neighW._x*scale._x,-neighW._y*scale._y,(DEMRaster.getValue(Engine::Point2D<int>(neighW._x,neighW._y))+offset));
               //glVertex3f(neighW._x*scale._x,-neighW._y*scale._y,1);
                glEnd();
            }

            //C-NW-N
            if (neighN._x < DEMRaster.getSize()._x && neighN._y < DEMRaster.getSize()._x && NW._x < DEMRaster.getSize()._x && NW._y < DEMRaster.getSize()._x
                      && PolygonInFrustum(Engine::Point3D<int>(neighN._x,neighN._y,DEMRaster.getValue(Engine::Point2D<int>(neighN._x,neighN._y))), Engine::Point3D<int>(NW._x,NW._y,DEMRaster.getValue(Engine::Point2D<int>(NW._x,NW._y))), Engine::Point3D<int>(center._x,center._y,DEMRaster.getValue(Engine::Point2D<int>(center._x,center._y)))))

            {
                glBegin(GL_TRIANGLES);

                if(DEMRaster.hasColorTable())
                {
                    Engine::ColorEntry color = DEMRaster.getColorEntry(DEMRaster.getValue(Engine::Point2D<int>(center._x,center._y)));
                    setCellColor(QColor(color._r, color._g, color._b, color._alpha));
                }
                else {
                    color = colorRaster.getValue(Engine::Point2D<int>(center._x,center._y));
                    setCellColor(colorSelector.getColor(color));
                }
                glTexCoord2f(center._x/size, center._y/size);
                glVertex3f(center._x*scale._x,-center._y*scale._y,(DEMRaster.getValue(Engine::Point2D<int>(center._x,center._y))+offset));
               //glVertex3f(center._x*scale._x,-center._y*scale._y,1);

                if(DEMRaster.hasColorTable())
                {
                    Engine::ColorEntry color = DEMRaster.getColorEntry(DEMRaster.getValue(Engine::Point2D<int>(neighN._x,neighN._y)));
                    setCellColor(QColor(color._r, color._g, color._b, color._alpha));
                }
                else {
                    color = colorRaster.getValue(Engine::Point2D<int>(neighN._x,neighN._y));
                    setCellColor(colorSelector.getColor(color));
                }
                glTexCoord2f(neighN._x/size, neighN._y/size);
                glVertex3f(neighN._x*scale._x,-neighN._y*scale._y,(DEMRaster.getValue(Engine::Point2D<int>(neighN._x,neighN._y))+offset));
               //glVertex3f(neighN._x*scale._x,-neighN._y*scale._y,1);

                if(DEMRaster.hasColorTable())
                {
                    Engine::ColorEntry color = DEMRaster.getColorEntry(DEMRaster.getValue(Engine::Point2D<int>(NW._x,NW._y)));
                    setCellColor(QColor(color._r, color._g, color._b, color._alpha));
                }
                else {
                    color = colorRaster.getValue(Engine::Point2D<int>(NW._x,NW._y));
                    setCellColor(colorSelector.getColor(color));
                }
                glTexCoord2f(NW._x/size, NW._y/size);
                glVertex3f(NW._x*scale._x,-NW._y*scale._y,(DEMRaster.getValue(Engine::Point2D<int>(NW._x,NW._y))+offset));
               //glVertex3f(NW._x*scale._x,-NW._y*scale._y,1);
                glEnd();
            }

            //C-N-NE
            if (neighN._x < DEMRaster.getSize()._x && neighN._y < DEMRaster.getSize()._x && NE._x < DEMRaster.getSize()._x && NE._y < DEMRaster.getSize()._x
                      && PolygonInFrustum(Engine::Point3D<int>(neighN._x,neighN._y,DEMRaster.getValue(Engine::Point2D<int>(neighN._x,neighN._y))),Engine::Point3D<int>(NE._x,NE._y,DEMRaster.getValue(Engine::Point2D<int>(NE._x,NE._y))),Engine::Point3D<int>(center._x,center._y,DEMRaster.getValue(Engine::Point2D<int>(center._x,center._y)))))
            {
                glBegin(GL_TRIANGLES);

                if(DEMRaster.hasColorTable())
                {
                    Engine::ColorEntry color = DEMRaster.getColorEntry(DEMRaster.getValue(Engine::Point2D<int>(center._x,center._y)));
                    setCellColor(QColor(color._r, color._g, color._b, color._alpha));
                }
                else {
                    color = colorRaster.getValue(Engine::Point2D<int>(center._x,center._y));
                    setCellColor(colorSelector.getColor(color));
                }
                glTexCoord2f(center._x/size, center._y/size);
                glVertex3f(center._x*scale._x,-center._y*scale._y,(DEMRaster.getValue(Engine::Point2D<int>(center._x,center._y))+offset));
               //glVertex3f(center._x*scale._x,-center._y*scale._y,1);

                if(DEMRaster.hasColorTable())
                {
                    Engine::ColorEntry color = DEMRaster.getColorEntry(DEMRaster.getValue(Engine::Point2D<int>(NE._x,NE._y)));
                    setCellColor(QColor(color._r, color._g, color._b, color._alpha));
                } else {
                    color = colorRaster.getValue(Engine::Point2D<int>(NE._x,NE._y));
                    setCellColor(colorSelector.getColor(color));
                }
                glTexCoord2f(NE._x/size, NE._y/size);
                glVertex3f(NE._x*scale._x,-NE._y*scale._y,(DEMRaster.getValue(Engine::Point2D<int>(NE._x,NE._y))+offset));
               //glVertex3f(NE._x*scale._x,-NE._y*scale._y,1);

                if(DEMRaster.hasColorTable())
                {
                    Engine::ColorEntry color = DEMRaster.getColorEntry(DEMRaster.getValue(Engine::Point2D<int>(neighN._x,neighN._y)));
                    setCellColor(QColor(color._r, color._g, color._b, color._alpha));
                } else {
                    color = colorRaster.getValue(Engine::Point2D<int>(neighN._x,neighN._y));
                    setCellColor(colorSelector.getColor(color));
                }
                glTexCoord2f(neighN._x/size, neighN._y/size);
                glVertex3f(neighN._x*scale._x,-neighN._y*scale._y,(DEMRaster.getValue(Engine::Point2D<int>(neighN._x,neighN._y))+offset));
               //glVertex3f(neighN._x*scale._x,-neighN._y*scale._y,1);
                glEnd();
            }

            //C-NE-E
            if (neighE._x < DEMRaster.getSize()._x && neighE._y < DEMRaster.getSize()._x && NE._x < DEMRaster.getSize()._x && NE._y < DEMRaster.getSize()._x
                      && PolygonInFrustum(Engine::Point3D<int>(neighE._x,neighE._y,DEMRaster.getValue(Engine::Point2D<int>(neighE._x,neighE._y))),Engine::Point3D<int>(NE._x,NE._y,DEMRaster.getValue(Engine::Point2D<int>(NE._x,NE._y))),Engine::Point3D<int>(center._x,center._y,DEMRaster.getValue(Engine::Point2D<int>(center._x,center._y)))))
            {
                glBegin(GL_TRIANGLES);

                if(DEMRaster.hasColorTable())
                {
                    Engine::ColorEntry color = DEMRaster.getColorEntry(DEMRaster.getValue(Engine::Point2D<int>(center._x,center._y)));
                    setCellColor(QColor(color._r, color._g, color._b, color._alpha));
                }
                else {
                    color = colorRaster.getValue(Engine::Point2D<int>(center._x,center._y));
                    setCellColor(colorSelector.getColor(color));
                }
                glTexCoord2f(center._x/size, center._y/size);
                glVertex3f(center._x*scale._x,-center._y*scale._y,(DEMRaster.getValue(Engine::Point2D<int>(center._x,center._y))+offset));
               //glVertex3f(center._x*scale._x,-center._y*scale._y,1);

                if(DEMRaster.hasColorTable())
                {
                    Engine::ColorEntry color = DEMRaster.getColorEntry(DEMRaster.getValue(Engine::Point2D<int>(neighE._x,neighE._y)));
                    setCellColor(QColor(color._r, color._g, color._b, color._alpha));
                }
                else {
                    color = colorRaster.getValue(Engine::Point2D<int>(neighE._x,neighE._y));
                    setCellColor(colorSelector.getColor(color));
                }
                glTexCoord2f(neighE._x/size, neighE._y/size);
                glVertex3f(neighE._x*scale._x,-neighE._y*scale._y,(DEMRaster.getValue(Engine::Point2D<int>(neighE._x,neighE._y))+offset));
               //glVertex3f(neighE._x*scale._x,-neighE._y*scale._y,1);

                if(DEMRaster.hasColorTable())
                {
                    Engine::ColorEntry color = DEMRaster.getColorEntry(DEMRaster.getValue(Engine::Point2D<int>(NE._x,NE._y)));
                    setCellColor(QColor(color._r, color._g, color._b, color._alpha));
                }
                else {
                    color = colorRaster.getValue(Engine::Point2D<int>(NE._x,NE._y));
                    setCellColor(colorSelector.getColor(color));
                }
                glTexCoord2f(NE._x/size, NE._y/size);
                glVertex3f(NE._x*scale._x,-NE._y*scale._y,(DEMRaster.getValue(Engine::Point2D<int>(NE._x,NE._y))+offset));
               //glVertex3f(NE._x*scale._x,-NE._y*scale._y,1);
                glEnd();
            }

            //C-E-SE
            if (neighE._x < DEMRaster.getSize()._x && neighE._y < DEMRaster.getSize()._x && SE._x < DEMRaster.getSize()._x && SE._y < DEMRaster.getSize()._x
                      && PolygonInFrustum(Engine::Point3D<int>(neighE._x,neighE._y,DEMRaster.getValue(Engine::Point2D<int>(neighE._x,neighE._y))),Engine::Point3D<int>(SE._x,SE._y,DEMRaster.getValue(Engine::Point2D<int>(SE._x,SE._y))),Engine::Point3D<int>(center._x,center._y,DEMRaster.getValue(Engine::Point2D<int>(center._x,center._y)))))
            {
                glBegin(GL_TRIANGLES);

                if(DEMRaster.hasColorTable())
                {
                    Engine::ColorEntry color = DEMRaster.getColorEntry(DEMRaster.getValue(Engine::Point2D<int>(center._x,center._y)));
                    setCellColor(QColor(color._r, color._g, color._b, color._alpha));
                }
                else {
                    color = colorRaster.getValue(Engine::Point2D<int>(center._x,center._y));
                    setCellColor(colorSelector.getColor(color));
                }
                glTexCoord2f(center._x/size, center._y/size);
                glVertex3f(center._x*scale._x,-center._y*scale._y,(DEMRaster.getValue(Engine::Point2D<int>(center._x,center._y))+offset));
               //glVertex3f(center._x*scale._x,-center._y*scale._y,1);

                if(DEMRaster.hasColorTable())
                {
                    Engine::ColorEntry color = DEMRaster.getColorEntry(DEMRaster.getValue(Engine::Point2D<int>(SE._x,SE._y)));
                    setCellColor(QColor(color._r, color._g, color._b, color._alpha));
                }
                else {
                    color = colorRaster.getValue(Engine::Point2D<int>(SE._x,SE._y));
                    setCellColor(colorSelector.getColor(color));
                }
                glTexCoord2f(SE._x/size, SE._y/size);
                glVertex3f(SE._x*scale._x,-SE._y*scale._y,(DEMRaster.getValue(Engine::Point2D<int>(SE._x,SE._y))+offset));
               //glVertex3f(SE._x*scale._x,-SE._y*scale._y,1);

                if(DEMRaster.hasColorTable())
                {
                    Engine::ColorEntry color = DEMRaster.getColorEntry(DEMRaster.getValue(Engine::Point2D<int>(neighE._x,neighE._y)));
                    setCellColor(QColor(color._r, color._g, color._b, color._alpha));
                }
                else {
                    color = colorRaster.getValue(Engine::Point2D<int>(neighE._x,neighE._y));
                    setCellColor(colorSelector.getColor(color));
                }
                glTexCoord2f(neighE._x/size, neighE._y/size);
                glVertex3f(neighE._x*scale._x,-neighE._y*scale._y,(DEMRaster.getValue(Engine::Point2D<int>(neighE._x,neighE._y))+offset));
               //glVertex3f(neighE._x*scale._x,-neighE._y*scale._y,1);
                glEnd();
            }

            //C-SE-S
            if (neighS._x < DEMRaster.getSize()._x && neighS._y < DEMRaster.getSize()._x && NE._x < DEMRaster.getSize()._x && NE._y < DEMRaster.getSize()._x
                      && PolygonInFrustum(Engine::Point3D<int>(neighS._x,neighS._y,DEMRaster.getValue(Engine::Point2D<int>(neighS._x,neighS._y))),Engine::Point3D<int>(SE._x,SE._y,DEMRaster.getValue(Engine::Point2D<int>(SE._x,SE._y))),Engine::Point3D<int>(center._x,center._y,DEMRaster.getValue(Engine::Point2D<int>(center._x,center._y)))))
            {
                glBegin(GL_TRIANGLES);

                if(DEMRaster.hasColorTable())
                {
                    Engine::ColorEntry color = DEMRaster.getColorEntry(DEMRaster.getValue(Engine::Point2D<int>(center._x,center._y)));
                    setCellColor(QColor(color._r, color._g, color._b, color._alpha));
                }
                else {
                    color = colorRaster.getValue(Engine::Point2D<int>(center._x,center._y));
                    setCellColor(colorSelector.getColor(color));
                }
                glTexCoord2f(center._x/size, center._y/size);
                glVertex3f(center._x*scale._x,-center._y*scale._y,(DEMRaster.getValue(Engine::Point2D<int>(center._x,center._y))+offset));
               //glVertex3f(center._x*scale._x,-center._y*scale._y,1);

                if(DEMRaster.hasColorTable())
                {
                    Engine::ColorEntry color = DEMRaster.getColorEntry(DEMRaster.getValue(Engine::Point2D<int>(neighS._x,neighS._y)));
                    setCellColor(QColor(color._r, color._g, color._b, color._alpha));
                }
                else {
                    color = colorRaster.getValue(Engine::Point2D<int>(neighS._x,neighS._y));
                    setCellColor(colorSelector.getColor(color));
                }
                glTexCoord2f(neighS._x/size, neighS._y/size);
                glVertex3f(neighS._x*scale._x,-neighS._y*scale._y,(DEMRaster.getValue(Engine::Point2D<int>(neighS._x,neighS._y))+offset));
               //glVertex3f(neighS._x*scale._x,-neighS._y*scale._y,1);

                if(DEMRaster.hasColorTable())
                {
                    Engine::ColorEntry color = DEMRaster.getColorEntry(DEMRaster.getValue(Engine::Point2D<int>(SE._x,SE._y)));
                    setCellColor(QColor(color._r, color._g, color._b, color._alpha));
                }
                else {
                    color = colorRaster.getValue(Engine::Point2D<int>(SE._x,SE._y));
                    setCellColor(colorSelector.getColor(color));
                }
                glTexCoord2f(SE._x/size, SE._y/size);
                glVertex3f(SE._x*scale._x,-SE._y*scale._y,(DEMRaster.getValue(Engine::Point2D<int>(SE._x,SE._y))+offset));
               //glVertex3f(SE._x*scale._x,-SE._y*scale._y,1);
                glEnd();
            }

            //C-S-SW
            if (neighS._x < DEMRaster.getSize()._x && neighS._y < DEMRaster.getSize()._x && SW._x < DEMRaster.getSize()._x && SW._y < DEMRaster.getSize()._x
                      && PolygonInFrustum(Engine::Point3D<int>(neighS._x,neighS._y,DEMRaster.getValue(Engine::Point2D<int>(neighS._x,neighS._y))),Engine::Point3D<int>(SW._x,SW._y,DEMRaster.getValue(Engine::Point2D<int>(SW._x,SW._y))),Engine::Point3D<int>(center._x,center._y,DEMRaster.getValue(Engine::Point2D<int>(center._x,center._y)))))
            {
                glBegin(GL_TRIANGLES);

                if(DEMRaster.hasColorTable())
                {
                    Engine::ColorEntry color = DEMRaster.getColorEntry(DEMRaster.getValue(Engine::Point2D<int>(center._x,center._y)));
                    setCellColor(QColor(color._r, color._g, color._b, color._alpha));
                }
                else {
                    color = colorRaster.getValue(Engine::Point2D<int>(center._x,center._y));
                    setCellColor(colorSelector.getColor(color));
                }
                glTexCoord2f(center._x/size, center._y/size);
                glVertex3f(center._x*scale._x,-center._y*scale._y,(DEMRaster.getValue(Engine::Point2D<int>(center._x,center._y))+offset));
               //glVertex3f(center._x*scale._x,-center._y*scale._y,1);

                if(DEMRaster.hasColorTable())
                {
                    Engine::ColorEntry color = DEMRaster.getColorEntry(DEMRaster.getValue(Engine::Point2D<int>(SW._x,SW._y)));
                    setCellColor(QColor(color._r, color._g, color._b, color._alpha));
                }
                else {
                    color = colorRaster.getValue(Engine::Point2D<int>(SW._x,SW._y));
                    setCellColor(colorSelector.getColor(color));
                }
                glTexCoord2f(SW._x/size, SW._y/size);
                glVertex3f(SW._x*scale._x,-SW._y*scale._y,(DEMRaster.getValue(Engine::Point2D<int>(SW._x,SW._y))+offset));
               //glVertex3f(SW._x*scale._x,-SW._y*scale._y,1);

                if(DEMRaster.hasColorTable())
                {
                    Engine::ColorEntry color = DEMRaster.getColorEntry(DEMRaster.getValue(Engine::Point2D<int>(neighS._x,neighS._y)));
                    setCellColor(QColor(color._r, color._g, color._b, color._alpha));
                }
                else {
                    color = colorRaster.getValue(Engine::Point2D<int>(neighS._x,neighS._y));
                    setCellColor(colorSelector.getColor(color));
                }
                glTexCoord2f(neighS._x/size, neighS._y/size);
                glVertex3f(neighS._x*scale._x,-neighS._y*scale._y,(DEMRaster.getValue(Engine::Point2D<int>(neighS._x,neighS._y))+offset));
               //glVertex3f(neighS._x*scale._x,-neighS._y*scale._y,1);
                glEnd();
            }

            //C-SW-W
            if (neighW._x < DEMRaster.getSize()._x && neighW._y < DEMRaster.getSize()._x && SW._x < DEMRaster.getSize()._x && SW._y < DEMRaster.getSize()._x
                      && PolygonInFrustum(Engine::Point3D<int>(neighW._x,neighW._y,DEMRaster.getValue(Engine::Point2D<int>(neighW._x,neighW._y))),Engine::Point3D<int>(SW._x,SW._y,DEMRaster.getValue(Engine::Point2D<int>(SW._x,SW._y))),Engine::Point3D<int>(center._x,center._y,DEMRaster.getValue(Engine::Point2D<int>(center._x,center._y)))))
            {
                glBegin(GL_TRIANGLES);

                if(DEMRaster.hasColorTable())
                {
                    Engine::ColorEntry color = DEMRaster.getColorEntry(DEMRaster.getValue(Engine::Point2D<int>(center._x,center._y)));
                    setCellColor(QColor(color._r, color._g, color._b, color._alpha));
                }
                else {
                    color = colorRaster.getValue(Engine::Point2D<int>(center._x,center._y));
                    setCellColor(colorSelector.getColor(color));
                }
                glTexCoord2f(center._x/size, center._y/size);
                glVertex3f(center._x*scale._x,-center._y*scale._y,(DEMRaster.getValue(Engine::Point2D<int>(center._x,center._y))+offset));
               //glVertex3f(center._x*scale._x,-center._y*scale._y,1);

                if(DEMRaster.hasColorTable())
                {
                    Engine::ColorEntry color = DEMRaster.getColorEntry(DEMRaster.getValue(Engine::Point2D<int>(neighW._x,neighW._y)));
                    setCellColor(QColor(color._r, color._g, color._b, color._alpha));
                }
                else {
                    color = colorRaster.getValue(Engine::Point2D<int>(neighW._x,neighW._y));
                    setCellColor(colorSelector.getColor(color));
                }
                glTexCoord2f(neighW._x/size, neighW._y/size);
                glVertex3f(neighW._x*scale._x,-neighW._y*scale._y,(DEMRaster.getValue(Engine::Point2D<int>(neighW._x,neighW._y))+offset));
               //glVertex3f(neighW._x*scale._x,-neighW._y*scale._y,1);

                if(DEMRaster.hasColorTable())
                {
                    Engine::ColorEntry color = DEMRaster.getColorEntry(DEMRaster.getValue(Engine::Point2D<int>(SW._x,SW._y)));
                    setCellColor(QColor(color._r, color._g, color._b, color._alpha));
                }
                else {
                    color = colorRaster.getValue(Engine::Point2D<int>(SW._x,SW._y));
                    setCellColor(colorSelector.getColor(color));
                }
                glTexCoord2f(SW._x/size, SW._y/size);
                glVertex3f(SW._x*scale._x,-SW._y*scale._y,(DEMRaster.getValue(Engine::Point2D<int>(SW._x,SW._y))+offset));
               //glVertex3f(SW._x*scale._x,-SW._y*scale._y,1);
                glEnd();
            }
            //glEnd();
            return prof;

        }
    }

}

void Quadtree::setCellColor( const QColor & color )
{

    float ambientColor[3];
    float diffuseColor[3];
    float specularColor[3];

    //QColor color(255,255,255,0);
    //QColor color(rand()%255,rand()%255,rand()%255);

        ambientColor[0] = color.redF();
        ambientColor[1] = color.greenF();
        ambientColor[2] = color.blueF();

        diffuseColor[0] = color.redF();
        diffuseColor[1] = color.greenF();
        diffuseColor[2] = color.blueF();

        specularColor[0] = specularColor[1] = specularColor[2] = 1.0f;
        glMaterialfv( GL_FRONT, GL_AMBIENT, ambientColor);
        glMaterialfv( GL_FRONT, GL_DIFFUSE, diffuseColor);
        glMaterialfv( GL_FRONT, GL_SPECULAR, specularColor);
}


}
