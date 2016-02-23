/**
 * @title  ImplementaciÛn de un grafo de escena.
 * @file   igvGrafoEscena.cpp
 * @author JosÈ ¡ngel Pastrana Padilla
 * @author JosÈ Ortega Donaire
 * @author Daniel MartÌnez Caballero
 */

#include "igvGrafoEscena.h"


// GRAFO DE ESCENA   ///////////////////////////////////////////////////

igvGrafoEscena::igvGrafoEscena(const unsigned int& _nGL): nGL(_nGL), gradoLibertad(new float[_nGL]), raiz(0) {
}

igvGrafoEscena::~igvGrafoEscena() {
    delete raiz;
    delete[] gradoLibertad;
}

void* igvGrafoEscena::insertar( void (*transformacion)(const float*), void (*color)(), void (*primitiva)(), void* padre) {
    if (primitiva==0) throw igvGrafoEscenaSinPrimitiva();
    if (padre!=0) {
        return ((igvGrafoNodo*) padre)->insertar(transformacion,color,primitiva);
    } else {
        if (raiz==0) {
            raiz = new igvGrafoNodo(transformacion,color,primitiva);
            return ((void*) raiz);
        } else {
            throw igvGrafoEscenaRaizOcupada();
        }
    }
}

void igvGrafoEscena::visualizar() const { 
    if (raiz) raiz->visualizar(gradoLibertad);
}

float igvGrafoEscena::getGL(const unsigned int& pos) const {
    if (pos<nGL)
        return gradoLibertad[pos];
    else
        throw igvGrafoEscenaFueraRango();
}

void igvGrafoEscena::setGL(const unsigned int& pos, const float& f) {
    if (pos<nGL)
        gradoLibertad[pos] = f;
    else
        throw igvGrafoEscenaFueraRango();
}

unsigned int igvGrafoEscena::totalGL() const {
    return nGL;
}

////////////////////////////////////////////////////////////////////////






// NODO DEL GRAFO   ////////////////////////////////////////////////////

igvGrafoEscena::igvGrafoNodo::igvGrafoNodo(void (*_transformacion)(const float*), void (*_color)(), void (*_primitiva)()): 
                                           transformacion(_transformacion),
                                           color(_color),
                                           primitiva(_primitiva),
                                           hijos() {
}

igvGrafoEscena::igvGrafoNodo::~igvGrafoNodo() {}

void* igvGrafoEscena::igvGrafoNodo::insertar(void (*_transformacion)(const float*), void(*_color)(), void (*_primitiva)()) {
    hijos.push_back(igvGrafoNodo(_transformacion,_color,_primitiva));
    return ((void*) &(hijos.back()));
}

void igvGrafoEscena::igvGrafoNodo::visualizar(const float* gradoLibertad) {
    glPushMatrix();
        if (transformacion) transformacion(gradoLibertad);
        if (color) color();
        primitiva();
        for( igvGrafoNodo igvGN : hijos ) igvGN.visualizar(gradoLibertad);
    glPopMatrix();
}

////////////////////////////////////////////////////////////////////////