//////////////////////////////////////////////////////////////////////////
// testMultipleCopies.cc.in
//
// garren@fnal.gov, January 2008
// Multiple events in memory at the same time
// run with valgrind or some other leak checker
//////////////////////////////////////////////////////////////////////////
//

#include <fstream>

#include "HepMC/IO_GenEvent.h"
#include "HepMC/GenEvent.h"
#include "HepMC/CompareGenEvent.h"

// define methods and classes used by this test
#include "IsGoodEvent.h"

int main() { 
    // use output file
    std::ofstream os( "testMultipleCopies.out" );
    {
	// declare an input strategy
	HepMC::IO_GenEvent ascii_in("./testIOGenEvent.input",std::ios::in);
	// declare another input strategy 
	HepMC::IO_GenEvent ascii_in2("./testHepMCVarious.input",std::ios::in);
	std::ofstream out1( "testMultipleOriginals.out" );
	std::ofstream out2( "testMultipleCopies1.out" );
	std::ofstream out3( "testMultipleCopies2.out" );
	// declare an instance of the event selection predicate
	IsGoodEvent is_good_event;

	//........................................EVENT LOOP
	int icount=0;
	int num_good_events=0;
	int icnt;
	HepMC::GenEvent* evt1 = ascii_in.read_next_event();
	HepMC::GenEvent* evt2 = ascii_in2.read_next_event();
	HepMC::GenEvent* evt3 = ascii_in.read_next_event();

	while ( evt1 && evt2 ) {
	    icount++;
	    if ( icount%50==1 ) os << "Processing Event Number " << icount
				   << " stream 1 # " << evt1->event_number() 
				   << " stream 2 # " << evt2->event_number() 
				   << std::endl;

	    if ( is_good_event(evt1) ) {

		os << "good event in stream 1 # " 
		   << evt1->event_number() << std::endl;
		evt1->print(out1);
		++num_good_events;
		HepMC::GenEvent ec = (*evt1);
		ec.print(out3);
		icnt=0;
		for ( HepMC::GenEvent::particle_const_iterator p1 =  ec.particles_begin();
        	      p1 !=  ec.particles_end(); ++p1 ) {
		      ++icnt;
		    os << "particle " << icnt << " barcode " <<(*p1)->barcode() << std::endl;
		}
		HepMC::GenEvent* evt4 = new HepMC::GenEvent(*evt1);
		evt4->print(out2);
 		if( !compareGenEvent(evt1,evt4) ) { return -1; }
        	delete evt4;
	    }

	    // clean up and get next events
	    delete evt1;
            delete evt2;
	    ascii_in >> evt1;
	    ascii_in2 >> evt2;
	}
	// might have either evt1 or evt2 still in memory, cleanup here
	delete evt1;
	delete evt2;
	delete evt3;

	//........................................PRINT RESULT
	os << std::endl;
	os << num_good_events << " out of " << icount 
	   << " processed events passed the cuts." << std::endl;
	os << std::endl;
	os << " GenEvent copy constructor passes the test" << std::endl;
	os << std::endl;
    }

    // test operator= and swap
    {
	// declare an input strategy
	HepMC::IO_GenEvent ascii_in("./testIOGenEvent.input",std::ios::in);
        //
        HepMC::GenEvent* evt5 = ascii_in.read_next_event();
        HepMC::GenEvent* evt6 = new HepMC::GenEvent();
        os << "event number for evt5: " << evt5->event_number() << std::endl;
        os << "event number for evt6: " << evt6->event_number() << std::endl;
	// copy  GenEvent object
        (*evt6) = (*evt5);
	if( !compareGenEvent(evt5,evt6) ) { return -4; }
        delete evt5;
        os << "event number for evt6 after copy: " << evt6->event_number() << std::endl;
	os << std::endl;
        delete evt6;
	os << " GenEvent operator= passes the test" << std::endl;
	os << std::endl;
	
	evt5 = ascii_in.read_next_event();
	evt6 = ascii_in.read_next_event();
        HepMC::GenEvent* evt7 = new HepMC::GenEvent(*evt5);
        HepMC::GenEvent* evt8 = new HepMC::GenEvent(*evt6);
        os << "event number for evt5: " << evt5->event_number() << std::endl;
        os << "event number for evt6: " << evt6->event_number() << std::endl;
        os << "before swap, evt5 has: " << evt5->vertices_size() << " vertices and "
	   << evt5->particles_size() << " particles" << std::endl;
        os << "before swap, evt6 has: " << evt6->vertices_size() << " vertices and "
	   << evt6->particles_size() << " particles" << std::endl;
        os << "before swap, evt7 has: " << evt7->vertices_size() << " vertices and "
	   << evt7->particles_size() << " particles" << std::endl;
        os << "before swap, evt8 has: " << evt8->vertices_size() << " vertices and "
	   << evt8->particles_size() << " particles" << std::endl;
	(*evt6).swap((*evt5));
        os << "event number for evt5 after swap: " << evt5->event_number() << std::endl;
        os << "event number for evt6 after swap: " << evt6->event_number() << std::endl;
        // evt6 should now match evt7
        os << "after swap, evt6 has: " << evt6->vertices_size() << " vertices and "
	   << evt6->particles_size() << " particles" << std::endl;
        os << "after swap, evt7 has: " << evt7->vertices_size() << " vertices and "
	   << evt7->particles_size() << " particles" << std::endl;
	if( !compareGenEvent(evt6,evt7) ) { return -6; }
        // evt5 should now match evt8
        os << "after swap, evt5 has: " << evt5->vertices_size() << " vertices and "
	   << evt5->particles_size() << " particles" << std::endl;
        os << "after swap, evt8 has: " << evt8->vertices_size() << " vertices and "
	   << evt8->particles_size() << " particles" << std::endl;
	if( !compareGenEvent(evt5,evt8) ) { return -5; }
	// cleanup
        delete evt5;
        delete evt6;
        delete evt7;
        delete evt8;
	os << std::endl;
	os << " GenEvent swap passes the test" << std::endl;
	os << std::endl;
    }
    return 0;
}
