/**
 * @title  ImplementaciÛn de un grafo de escena.
 * @file   igvGrafoEscena.h
 * @author JosÈ ¡ngel Pastrana Padilla
 * @author JosÈ Ortega Donaire
 * @author Daniel MartÌnez Caballero
 */

#ifndef IGVGRAFOESCENA_H
#define	IGVGRAFOESCENA_H

    #include <GL/glut.h>

#include <list>
#include <exception>

/**
 * @brief Estructura que permite visualizar objetos complejos (modelos) y modificarlos mediante sus grados de libertad.
 */
class igvGrafoEscena {
private:
    class igvGrafoNodo; /**< PredeclaraciÛn de un nodo del grafo. */
    
    const unsigned int nGL; /**< Cantidad de grados de libertad del grafo. */
    float* gradoLibertad; /**< Vector con los valores de los grados de libertad. */
    
    igvGrafoNodo* raiz; /**< RaÌz del grafo de escena. */
    
public:
    
    /**
     * @brief Constructor de grafo de escena.
     * @param nGL Cantidad de grado de libertad disponibles para este grafo. Por defecto, 0.
     */
    igvGrafoEscena(const unsigned int& nGL = 0);
    
    /**
     * @brief Destructor del grafo de escena.
     */
    virtual ~igvGrafoEscena();
    
    /**
     * @brief Inserta un nuevo elemento para el objeto compuesto en el grafo de escena.
     * @param transformacion Puntero a una funciÛn de transformaciones para el nuevo elemento.
     * @param color Puntero a una funciÛn que establece el color para la primitiva.
     * @param primitiva Puntero a una funciÛn de primitiva para el nuevo elemento.
     * @param padre DirecciÛn de memoria del elemento padre.
     * @return DirecciÛn de memoria del nuevo elemento tras la inserciÛn.
     */
    void* insertar( void (*transformacion)(const float*), void (*color)(), void (*primitiva)(), void* padre = 0 );
    
    /**
     * @brief Genera la figura modelada en el grafo.
     */
    void visualizar() const;
    
    /**
     * @brief Obtiene un grado de libertad concreto.
     * @param pos PosiciÛn deseada a consultar.
     * @return Un valor en coma flotante para la posiciÛn consultada.
     */
    float getGL(const unsigned int& pos) const;
    
    /**
     * @brief Establece un grado de libertad concreto.
     * @param pos PosiciÛn deseada a establecer.
     * @param f Un valor en coma flotante para la posiciÛn establecida.
     */
    void setGL(const unsigned int& pos, const float& f);
    
    /**
     * @brief Cantidad de grados de libertad diferentes disponibles en el grafo de escena.
     */
    unsigned int totalGL() const;
};


/**
 * @brief Componente del grafo de escena que representa un elemento para el objeto compuesto en el grafo de escena.
 */
class igvGrafoEscena::igvGrafoNodo {
private:
    void (*transformacion)(const float*); /**< Puntero a una funciÛn que realiza transformaciones a la primitiva dada. */
    void (*color)(); /**< Puntero a una funciÛn que establece el color para la primitiva dada. */
    void (*primitiva)(); /**< Puntero a una funciÛn que pinta en GLUT alg˙n elemento. */
    std::list<igvGrafoNodo> hijos; /**< Lista enlazada con los descendientes de este elemento. */
    
public:
    /**
     * @brief Constructor de un nodo del grafo de escena.
     * @param _transformacion Puntero a una funciÛn que realiza transformaciones a la primitiva dada.
     * @param _color Puntero a una funciÛn que establece el color para la primitiva dada.
     * @param _primitiva Puntero a una funciÛn que pinta en GLUT alg˙n elemento.
     */
    igvGrafoNodo(void (*_transformacion)(const float*), void (*_color)(), void (*_primitiva)());
    
    /**
     * @brief Destructor de un nodo del grafo de escena.
     */
    virtual ~igvGrafoNodo();
    
    /**
     * @brief Inserta un nuevo elemento al grafo como descendiente de este elemento.
     * @param _transformacion Puntero a una funciÛn que realiza trasnformaciones correspondiente al nuevo elemento.
     * @param _color Puntero a una funuciÛn que establece el color para la primitiva.
     * @param _primitiva Puntero a una funciÛn que pinta en GLUT correspondiente al nuevo elemento.
     * @return DirecciÛn de memoria del nuevo elemento tras la inserciÛn.
     */
    void* insertar(void (*_transformacion)(const float*), void (*_color)(), void (*_primitiva)());
    
    /**
     * @brief Visualiza el elemento actual y sus descendientes.
     * @param gradoLibertad Vector con los grados de libertad que se asocian a las primitivas del elemento y sus descendientes.
     */
    void visualizar(const float* gradoLibertad);
};

/**
 * @brief ExcepciÛn de clase igvGrafoEscena generada cuando se inserta una primitiva nula.
 */
class igvGrafoEscenaSinPrimitiva: public std::exception {
    virtual const char* what() const throw() {
        return "igvGrafoEscena: El elemento insertado hace una llamada a una primitiva no declarada.";
    }
};

/**
 * @brief ExcepciÛn de clase igvGrafoEscena generada cuando la raÌz del grafo de escena ya est· ocupada.
 */
class igvGrafoEscenaRaizOcupada: public std::exception {
    virtual const char* what() const throw() {
        return "igvGrafoEscena: RaÌz del grafo de escena ya est· ocupada y no se puede volver a reescribir.";
    }
};

/**
 * @brief ExcepciÛn de clase igvGrafoEscena generada cuando se tiene acceso a una posiciÛn inv·lida en el vector de grados de libertad.
 */
class igvGrafoEscenaFueraRango: public std::exception {
    virtual const char* what() const throw() {
        return "igvGrafoEscena: Acceso a una posiciÛn inv·lida del vector de grados de libertad.";
    }
};

#endif	/* IGVGRAFOESCENA_H */

