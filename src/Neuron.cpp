#include "Neuron.hpp"
#include <cmath>
#include <string>
#include <assert.h>
#include <random>

unsigned int Neuron::neuron_id_ = 0;
Physics::Potential const Neuron::firing_threshold_= 20;
Physics::Potential const Neuron::rest_potential_= 10;
Physics::Time const Neuron::transmission_delay_= 2; // vraie valeur est 1.5
Physics::Time const Neuron::tau_ = 20;
Physics::Amplitude const Neuron::amplitude_= 0.1;


using namespace std;


Neuron::Neuron(Type const& a_type, bool const& exc, double const& eps,
				double const& external_factor, Physics::Resistance const& membrane_resistance, double Vm, double I, Physics::Time refractory_period, Physics::Time t)
				: type_(a_type), excitatory_(exc), inhib_connections_(250), excit_connections_(1000),
				epsilon_(eps), external_factor_(external_factor), membrane_resistance_(membrane_resistance), Vm_(Vm), I_(I), refractory_period_(refractory_period), t_(t)

{
    std::priority_queue <Event> ev;
    events_in_ = ev; // on initialise events_in_ à un tableau vide
    
    string fileName =  "neuron_" + to_string(neuron_id_) + ".csv";
    neuron_file = new ofstream(fileName);

    
    if (neuron_file->fail()) {
        throw string("Error: The file doesn't exist !");
    } else {
        *neuron_file << "t [ms]" << "," << "Vm [V]" << endl;
    }

    ++neuron_id_;
}



Neuron::~Neuron()
{  
    neuron_file->close();
    delete neuron_file;
}



void Neuron::input(Physics::Time const& dt)
{
	//ANALYTIC
	if(type_ == Type::Analytic)
	{
		t_ += dt + transmission_delay_;
		update_RI(dt+transmission_delay_);
		step(dt+transmission_delay_);
	}

	else
	{
		update_RI(dt);
		step(dt);
    }
}



void Neuron::output(double const& x)
{

    Event ev(t_, x);

    for (size_t i = 0; i < synapses_.size(); ++i)
    {
        assert(synapses_[i]!=NULL);
        synapses_[i]->add_event_in(ev);
    }
}




bool Neuron::has_reached_threshold() const
{
    return Vm_ >= firing_threshold_;
}


void Neuron::add_event_in(Event const& ev)
{
	events_in_.push(ev);
}





// remet le potentiel de membrane au potentiel au repos, cette méthode sera appelée
// dans update si le threshold est dépassé
void Neuron::reset_potential()
{
    Vm_ = rest_potential_;
}


void external_spike_generator()
{
	/// Construct a random generator engine from a time-based seed
	auto seed = std::chrono::system_clock::now().time_since_epoch().count();
	std::default_random_engine generator(seed);

	/// Use a Poisson distribution with a rate 
	std::poisson_distribution distribution(external_factor_*Ce_*dt);
	distribution(generator);
			
	
}


void Neuron::update(Physics::Time const& dt)
{

    input(dt); //<met d'abord à jour les input (ce que le neurone reçoit)
    //<décrémenter refractory period jusqu'à 0 pas en dessous
    //< output à toutes ses connexions dans le cas où le threshold est atteint
    //< et le courant est remis à 0
    //< output à toutes ses connexions dans le cas où le threshold est atteint
    //< et le courant est remis à 0

    if (has_reached_threshold())
    {
        output(I_);
        reset_potential();
        I_ = 0;
        refractory_period_ = 2;
    }
   
    *neuron_file << t_ << "," << Vm_ << endl;
    
}




void Neuron::set_connection(Neuron* neuron)
{
        assert(neuron!=NULL);
	synapses_.push_back(neuron);
}

void Neuron::step(Physics::Time const& dt)
{

	switch(type_)
	{
		case Type::Analytic :
		step_analytic(dt);
		break;

		case Type::Explicit :
		step_explicit(dt);
		t_ += dt;
		break;

		case Type::Implicit :
		step_implicit(dt);
		t_ += dt;
		break;
	}

}


void Neuron::step_analytic(Physics::Time const& dt)
{

	Vm_ = membrane_resistance_ * I_*exp(-dt/tau_);

}


void Neuron::step_explicit(Physics::Time const& dt)// Use of V(t-1)=Vm_ to calculate the new Vm_
{
	Vm_ += ((-Vm_ + membrane_resistance_ * I_) * dt) / tau_;
}


void Neuron::step_implicit(Physics::Time const& dt)
{
	Vm_ = ((dt * membrane_resistance_ * I_ ) + (tau_ * Vm_)) / ( dt + tau_);
}


void Neuron::update_RI(Physics::Time const& dt)
{
	if(type_ == Type::Analytic)
	{
		while( (!events_in_.empty()) && (refractory_period_ == 0) && (Physics::dirac_distribution(t_- transmission_delay_ - events_in_.top().get_t()) == 1) )
		{
			I_ += events_in_.top().get_i();
			events_in_.pop();
		}


	}

	else if ((type_ == Type::Explicit) or (type_ == Type::Implicit))
	{
        
		while((!events_in_.empty()) && (events_in_.top().get_t() < (t_ + dt)) && (refractory_period_ == 0))
		{
		   
		// si la différence entre le temps courant et (transmission_delay_ + temps où le courant a été envoyé) = 0
		// la fonction de dirac retourne 1 et dans ce cas on incrémente le courant
			if (Physics::dirac_distribution(t_- transmission_delay_ - events_in_.top().get_t()) == 1)
			{
				
				I_ += amplitude_ / membrane_resistance_;
				events_in_.pop();

			}

		}

	}
}


Physics::Time Neuron::get_t() const
{
	return t_;
	
}



double Neuron::get_Vm() const
{
	return Vm_;

}

double Neuron::get_I() const
{
	return I_;

}

int Neuron::get_synapses_size() const
{
	return synapses_.size();
}

int Neuron::get_event_in_size() const
{
	return events_in_.size();
}
