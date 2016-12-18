#ifndef PHYSICS_HPP
#define PHYSICS_HPP

//see page 9 here for parameters:
//http://arken.nmbu.no//~plesser/publications/Gewa_2012_533_preprint.pdf
#define FIRING_THRESHOLD 20
#define RESTING_POTENTIAL 0
#define RESET_POTENTIAL 10
#define REFRACTORY_PERIOD 2
#define TRANSMISSION_DELAY 1.5
#define TAU 20
#define WEIGHT_J_EXC 0.1
#define GAMMA 0.25 //1 to 4 ratio between Inh and Exc neurons count
#define EPSILON 0.1 //10% connectivity
#define RANDOM_SEED 0 //default random seed (if called from command line, is random)
#define EXTERNAL_FACTOR 2 //20 spikes per ms (see Nest Software wiki page, or eta variable in paper above)
#define NUMBER_OF_NEURONS 12500
#define SIMULATION_TIME 1000 //ms
#define TIME_STEP 0.1 //ms
#define SPIKE_INTERVAL 1 //ms
#define RELATIVE_STRENGTH 5.0 //no unit

enum class SimulationType : short
{
    AnalyticFixedStep, AnalyticVariableStep, ExplicitForwardEuler, ImplicitBackwardEuler
};

/**
 * @namespace Physics units and constants
 */
namespace Physics
{
    using Potential  = double; ///< Voltage unit (mV)
    using Time       = double; ///< Time unit (ms)
    using Resistance = double; ///< Resistance unit (ohm)
    using Amplitude  = double; ///< Amplitude unit
}

#endif // PHYSICS_HPP
