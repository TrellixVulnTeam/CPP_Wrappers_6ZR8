//-------------------------------------------------------------------
// testMass.cc.in
//
// garren@fnal.gov, March 2006
// Read events written by example_MyPythia.cc
// Select events containing a photon of pT > 25 GeV 
// Add arbitrary PDF information to one of the good events
// Add arbitrary HeavyIon information to one of the good events
// Write the selected events and read them back in using an istream
//-------------------------------------------------------------------

#include <cmath>	// for min()
#include <ostream>

#include "HepMC/IO_GenEvent.h"
#include "HepMC/GenEvent.h"
#include "HepMC/Version.h"

// define methods and classes used by this test
#include "IsGoodEvent.h"

void   massInfo( const HepMC::GenEvent*, std::ostream& os );

int main() { 
    // output file
    std::ofstream os( "testMass.cout" );
    // read and process the input file
    {
	// declare an input strategy to read the data produced with the 
	// example_MyPythia
        HepMC::IO_GenEvent ascii_in("./testIOGenEvent.input",std::ios::in);
        ascii_in.use_input_units( HepMC::Units::GEV, HepMC::Units::MM );
	// declare another IO_GenEvent for output
	HepMC::IO_GenEvent ascii_out("testMass1.out",std::ios::out);
	// declare an instance of the event selection predicate
	IsGoodEvent is_good_event;
	// send version to output
	HepMC::version(os);
	//........................................EVENT LOOP
	int icount=0;
	int num_good_events=0;
        double x1=0., x2=0., q=0., xf1=0., xf2=0.;
	HepMC::GenEvent* evt = ascii_in.read_next_event();
	while ( evt ) {
	    icount++;
	    if ( icount%50==1 ) os << "Processing Event Number " << icount
				   << " its # " << evt->event_number() 
				   << std::endl;
	    if ( is_good_event(evt) ) {
		if (num_good_events == 0 ) {
		    // add some arbitrary PDF information
		    x1 = std::min(0.8, 0.07 * icount);
		    x2 = 1-x1;
		    q = 1.69 * icount;
		    // use beam momentum
		    if( evt->valid_beam_particles() ) {
		        HepMC::GenParticle* bp1 = evt->beam_particles().first;
			xf1 = x1*bp1->momentum().rho();
			xf2 = x2*bp1->momentum().rho();
		    } else {
			xf1 = x1*0.34;
			xf2 = x2*0.34;
		    }
		    // provide optional pdf set id numbers 
		    // (two ints at the end of the constructor)
		    HepMC::PdfInfo pdf( 2, 3, x1, x2, q, xf1, xf2, 230, 230);
		    evt->set_pdf_info(pdf);
		    // add some arbitrary HeavyIon information
	            HepMC::HeavyIon ion(23,11,12,15,3,5,0,0,0,0.0145);
	            evt->set_heavy_ion( ion );
		}
	        os << "saving Event " << evt->event_number() << std::endl;
		if( evt->weights().size() > 0 ) {
		    os << "Weights: ";
		    evt->weights().print(os);
		}
		ascii_out << evt;
		++num_good_events;
	    }

	    // clean up and get next event
	    delete evt;
	    ascii_in >> evt;
	}
	//........................................PRINT RESULT
	os << num_good_events << " out of " << icount 
	   << " processed events passed the cuts. Finished." << std::endl;
    }
    // now read the file we just created
    {
	// declare an input strategy 
	const char infile[] = "testMass1.out";
	std::ifstream istr( infile );
	if( !istr ) {
	  std::cerr << "testMass: cannot open " << infile << std::endl;
	  exit(-1);
	}
	HepMC::IO_GenEvent xin(istr);
	// declare another IO_GenEvent for output
	HepMC::IO_GenEvent xout("testMass2.out",std::ios::out);
	//........................................EVENT LOOP
	int ixin=0;
	HepMC::GenEvent* evt = xin.read_next_event();
	while ( evt ) {
	    ixin++;
	    os << "reading Event " << evt->event_number() << std::endl;
	    if( evt->weights().size() > 0 ) {
		os << "Weights: ";
		evt->weights().print(os);
	    }
	    xout << evt;
	    // look at mass info
	    massInfo(evt,os);

	    // clean up and get next event
	    delete evt;
	    xin >> evt;
	}
	//........................................PRINT RESULT
	os << ixin << " events in the second pass. Finished." << std::endl;
    }
}

void massInfo( const HepMC::GenEvent* e, std::ostream& os )
{
   double gm, m, d;
   for ( HepMC::GenEvent::particle_const_iterator p = e->particles_begin(); p != e->particles_end();
         ++p ) {
	 
       gm = (*p)->generated_mass();
       m = (*p)->momentum().m();
       d = fabs(m-gm);
       if( d > 1.0e-5 ) {
	    os << "Event " << e->event_number()
	       << " Particle " << (*p)->barcode() 
	       << " " << (*p)->pdg_id()
               << " generated mass " << gm
               << " mass from momentum " << m 
	       << " difference " << d << std::endl;
       } 
   }   
}
