#include "trick/Integrator.hh"
#include "trick/message_proto.h"
#include "trick/message_type.h"
#include <cstdarg>
#include <iostream>

/**
 */
Trick::Integrator::Integrator() {

   num_state = 0;
   first_step_deriv = 1;
   last_step_deriv  = 0;
   intermediate_step = 0;
   is_2nd_order_ODE_technique = 0;
   use_deriv2 = 0;
   dt = 0.01;
   state = NULL;
   deriv = NULL;
   deriv2 = NULL;
   state_ws = NULL;
   time = 0.0;
   time_0 = 0.0;
   verbosity = 0 ;
}

/**
 */
int Trick::Integrator::integrate_1st_order_ode (
    double const* derivs_in, double* state_in_out) {
    int rc;

    if (is_2nd_order_ODE_technique) {
        message_publish(MSG_ERROR,
                        "Integrator ERROR: "
                        "Current integrator is a 2nd order technique.\n");
        return 0;
    }

    for (int ii = 0; ii < num_state; ++ii) {
        state[ii] = state_in_out[ii];
        deriv[intermediate_step][ii] = derivs_in[ii];
    }

    rc = integrate ();

    for (int ii = 0; ii < num_state; ++ii) {
        state_in_out[ii] = state_ws[intermediate_step][ii];
    }

    return rc;
}

/**
 */
int Trick::Integrator::integrate_2nd_order_ode (
    double const* accel, double* velocity, double* position) {
    int rc;
    int half_size = num_state / 2;

    if (use_deriv2) {
        for (int ii = 0; ii < half_size; ++ii) {
            state[ii] = position[ii];
            deriv[intermediate_step][ii]  = velocity[ii];
            deriv2[intermediate_step][ii] = accel[ii];
        }
    }

    else {
        for (int ii = 0; ii < half_size; ++ii) {
            state[ii]           = position[ii];
            state[ii+half_size] = velocity[ii];
            deriv[intermediate_step][ii]           = velocity[ii];
            deriv[intermediate_step][ii+half_size] = accel[ii];
        }
    }

    rc = integrate ();

    for (int ii = 0; ii < half_size; ++ii) {
        position[ii] = state_ws[intermediate_step][ii];
        velocity[ii] = state_ws[intermediate_step][ii+half_size];
    }

    return rc;
}

/**
 */
void Trick::Integrator::state_in (double* arg1, ...) {

    va_list ap;
    int i;
    double* narg;

    va_start(ap, arg1);

    i=0;
    narg = arg1;
    if (verbosity) message_publish(MSG_DEBUG, "LOAD STATE: ");
    while (narg != (double*)NULL) {
        state[i] = *narg;
        if (verbosity) message_publish(MSG_DEBUG,"  %g", *narg);
        narg = va_arg(ap, double*);
        i++;
    };
    if (verbosity) message_publish(MSG_DEBUG,"\n");
    va_end(ap);
}

/**
 */
void Trick::Integrator::deriv_in ( double* arg1, ...) {

    va_list ap;
    int i;
    double* narg;

    va_start(ap, arg1);

    i=0;
    narg = arg1;
    if (verbosity) message_publish(MSG_DEBUG,"LOAD DERIV: ");
    while (narg != (double*)NULL) {
        deriv[intermediate_step][i] = *narg;
        if (verbosity) message_publish(MSG_DEBUG,"  %g",*narg);
        narg = va_arg(ap, double*);
        i++;
    };
    va_end(ap);
    if (verbosity) message_publish(MSG_DEBUG,"\n");
}

/**
  */
void Trick::Integrator::deriv2_in ( double* arg1, ...) {

    va_list ap;
    int i;
    double* narg;

    va_start(ap, arg1);

    i=0;
    narg = arg1;
    if (verbosity) message_publish(MSG_DEBUG,"LOAD DERIV2: ");
    while (narg != (double*)NULL) {
        deriv2[intermediate_step][i] = *narg;
        if (verbosity) message_publish(MSG_DEBUG,"  %g",*narg);
        narg = va_arg(ap, double*);
        i++;
    };
    if (verbosity) message_publish(MSG_DEBUG,"\n");
    va_end(ap);
}


/**
 */
void Trick::Integrator::state_out (double* arg1, ...) {

    va_list ap;
    int i;
    double* narg;

    va_start(ap, arg1);

    i=0;
    narg = arg1;
    if (verbosity) message_publish(MSG_DEBUG,"UNLOAD STATE: ");
    while (narg != (double*)NULL) {
        *narg = state_ws[intermediate_step][i];
        if (verbosity) message_publish(MSG_DEBUG,"  %g",*narg);
        narg = va_arg(ap, double*);
        i++;
    };
    if (verbosity) message_publish(MSG_DEBUG,"\n");
    va_end(ap);
}

bool Trick::Integrator::get_first_step_deriv() {
    return (first_step_deriv);
}

void Trick::Integrator::set_first_step_deriv(bool first_step) {
    first_step_deriv = first_step;
}

bool Trick::Integrator::get_last_step_deriv() {
    return (last_step_deriv);
}

void Trick::Integrator::set_last_step_deriv(bool last_step) {
    last_step_deriv = last_step;
}

void Trick::Integrator::set_verbosity(int level) {
    verbosity = level;
}
