/*
Functions for implementing a Gauss-Legendre Quadrature numerical integration
(Hildebrand, 1987).

Usage example
-------------

To integrate the cossine function from 0 to 90 degrees:

    #include <stdio.h>
    #include <stdlib.h>
    #include <math.h>
    #include "src/c/glq.h"

    int main(){
        // Create a new glq structure
        GLQ *glq;
        double result = 0, a = 0, b = 0.5*3.14;
        int i;

        glq = glq_new(5, a, b);

        if(glq == NULL){
            printf("malloc error");
            return 1;
        }

        // Calculate the integral
        for(i = 0; i < glq->order; i++)
            result += glq->weights[i]*cos(glq->nodes[i]);

        // Need to multiply by a scale factor of the integration limits
        result *= 0.5*(b - a);

        printf("Integral of cossine from 0 to 90 degrees = %lf\n", result);

        // Free allocated memory
        glq_free(glq);

        return 0;
    }

References
----------

* Hildebrand, F.B (1987): Introduction to numerical analysis.
  Courier Dover Publications, 2. ed.
*/

#ifndef _TESSEROIDS_GLQ_H_
#define _TESSEROIDS_GLQ_H_


/** \var GLQ_MAXIT
Max iterations of the root-finder algorithm */
const int GLQ_MAXIT = 1000;


/** \var GLQ_MAXERROR
Max error allowed for the root-finder algorithm */
const double GLQ_MAXERROR = 0.000000000000001;


/** Store the nodes and weights needed for a GLQ integration */
typedef struct glq_struct
{
    int order; /**< order of the quadrature, ie number of nodes */
    double *nodes; /**< abscissas or discretization points of the quadrature */
    double *weights; /**< weighting coefficients of the quadrature */
    double *nodes_unscaled; /**< nodes in [-1,1] interval */
} GLQ;


/** Make a new GLQ structure and set all the parameters needed

<b>WARNING</b>: Don't forget to free the memory malloced by this function using
glq_free()!

Prints error and warning messages using the logging.h module.

@param order order of the quadrature, ie number of nodes
@param lower lower integration limit
@param upper upper integration limit

@return GLQ data structure with the nodes and weights calculated. NULL if there
    was an error with allocation.
*/
GLQ * glq_new(int order, double lower, double upper);


/** Free the memory allocated to make a GLQ structure

@param glq pointer to the allocated memory
*/
void glq_free(GLQ *glq);


/** Put the GLQ nodes to the integration limits <b>IN PLACE</b>.

Will replace the values of glq.nodes with ones in the specified integration
limits.

In case the GLQ structure was created with glq_new(), the integration limits can
be reset using this function.

@param lower lower integration limit
@param upper upper integration limit
@param glq pointer to a GLQ structure created with glq_new() and with all
           necessary memory allocated

@return Return code:
    - 0: if everything went OK
    - 1: if invalid order
    - 2: if NULL pointer for nodes or nodes_unscaled
*/
int glq_set_limits(double lower, double upper, GLQ *glq);
/** Calculates the GLQ nodes using glq_next_root.

Nodes will be in the [-1,1] interval. To convert them to the integration limits
use glq_scale_nodes

@param order order of the quadrature, ie how many nodes. Must be >= 2.
@param nodes pre-allocated array to return the nodes.

@return Return code:
    - 0: if everything went OK
    - 1: if invalid order
    - 2: if NULL pointer for nodes
    - 3: if number of maximum iterations was reached when calculating the root.
         This usually means that the desired accuracy was not achieved. Default
         desired accuracy is GLQ_MAXERROR. Default maximum iterations is
         GLQ_MAXIT.
*/
int glq_nodes(int order, double *nodes);


/** Calculate the next Legendre polynomial root given the previous root found.

Uses the root-finder algorithm of:

  Barrera-Figueroa, V., Sosa-Pedroza, J. and López-Bonilla, J., 2006,
  "Multiple root finder algorithm for Legendre and Chebyshev polynomials via
  Newton's method", 2006, Annales mathematicae et Informaticae, 33, pp 3-13

@param initial initial estimate of the next root. I recommend the use of
               \f$ \cos\left(\pi\frac{(N - i - 0.25)}{N + 0.5}\right) \f$,
               where \f$ i \f$ is the index of the desired root
@param root_index index of the desired root, starting from 0
@param order order of the Legendre polynomial, ie number of roots.
@param roots array with the roots found so far. Will return the next root in
             roots[root_index], so make sure to malloc enough space.

@return Return code:
    - 0: if everything went OK
    - 1: if order is not valid
    - 2: if root_index is not valid (negative)
    - 3: if number of maximum iterations was reached when calculating the root.
         This usually means that the desired accuracy was not achieved. Default
         desired accuracy is GLQ_MAXERROR. Default maximum iterations is
         GLQ_MAXIT.
*/
int glq_next_root(double initial, int root_index, int order,
                         double *roots);


/** Calculates the weighting coefficients for the GLQ integration.

@param order order of the quadrature, ie number of nodes and weights.
@param nodes array containing the GLQ nodes calculated by glq_nodes.
             <b>IMPORTANT</b>: needs the nodes in [-1,1] interval! Scaled nodes
             will result in wrong weights!
@param weights pre-allocated array to return the weights

@return Return code:
    - 0: if everything went OK
    - 1: if order is not valid
    - 2: if nodes is a NULL pointer
    - 3: if weights is a NULL pointer
*/
int glq_weights(int order, double *nodes, double *weights);

#endif
