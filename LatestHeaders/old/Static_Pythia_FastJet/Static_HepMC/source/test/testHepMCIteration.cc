//////////////////////////////////////////////////////////////////////////
// testHepMCIteration.cc.in
//
// garren@fnal.gov, May 2007
// Use Matt's example_EventSelection along with example_UsingIterators 
// to check HepMC iteration.
// Apply an event selection to the events in testHepMC.input
// Events containing a photon of pT > 25 GeV pass the selection.
// Use iterators on these events.
//////////////////////////////////////////////////////////////////////////

#include <list>

#include "HepMC/IO_GenEvent.h"
#include "HepMC/IO_AsciiParticles.h"
#include "HepMC/GenEvent.h"
#include "HepMC/GenRanges.h"

// define methods and classes used by this test
#include "IsGoodEvent.h"
#include "testHepMCIteration.h"

bool findW( HepMC::GenEvent* evt, std::ofstream& os);
bool simpleIter ( HepMC::GenEvent* evt, std::ostream& os = std::cout );
bool simpleIter2( HepMC::GenEvent* evt, std::ostream& os = std::cout );
bool simpleIter3( HepMC::GenEvent* evt, std::ostream& os = std::cout );
bool simpleIter4( HepMC::GenEvent* evt, std::ostream& os = std::cout );

/// \class  PrintW
/// This class is designed for use with std::for_each
/// This class also illustrates the use of the new GenEventIterator classes.
/// PrintW duplicates the funtionality within findW's particle loop.
class PrintW {
public:
    PrintW( std::ostream & os, int num ) : m_out( os ),m_event_num( num ) {}
    void operator()( HepMC::GenParticle* p ) { 
	if ( IsWBoson(p) ) {
	    m_out << std::endl;
	    m_out << "A W boson has been found in event: " << m_event_num << std::endl;
	    p->print( m_out );
	    // return all parents
	    // we do this by pointing to the production vertex of the W 
	    // particle and asking for all particle parents of that vertex
	    m_out << "\t Its parents are: " << std::endl;
	    if ( p->production_vertex() ) {
 		std::for_each( p->particles_in(HepMC::parents).begin(),
		               p->particles_in(HepMC::parents).end(), 
			       PrintParticle(m_out));
	    }

	    // return immediate children
	    m_out << "\t\t" << "Its children are: " << std::endl;
	    if ( p->end_vertex() ) {
		std::for_each( p->particles_out(HepMC::children).begin(), 
		               p->particles_out(HepMC::children).end(), 
			       PrintChildren(m_out));
	    }

	    // return all descendants
	    // we do this by pointing to the end vertex of the W 
	    // particle and asking for all particle descendants of that vertex
	    m_out << "\t\t Its descendants are: " << std::endl;
	    if ( p->end_vertex() ) {
		std::for_each( p->particles_out(HepMC::descendants).begin(), 
		               p->particles_out(HepMC::descendants).end(), 
			       PrintDescendants(m_out));
	    }
	}	// if IsWBoson
    }
private:
   std::ostream & m_out;
   int            m_event_num;
};

/// \class  PrintConstW
/// This class is designed for use with std::for_each
/// This class also illustrates the use of the new GenEventIterator classes.
/// PrintConstW duplicates the funtionality within findW's particle loop.
class PrintConstW {
public:
    PrintConstW( std::ostream & os, int num ) : m_out( os ),m_event_num( num ) {}
    void operator()( HepMC::GenParticle* p ) { 
	if ( IsWBoson(p) ) {
	    m_out << std::endl;
	    m_out << "A W boson has been found in event: " << m_event_num << std::endl;
	    p->print( m_out );
	    // return all parents
	    // we do this by pointing to the production vertex of the W 
	    // particle and asking for all particle parents of that vertex
	    m_out << "\t Its parents are: " << std::endl;
	    if ( p->production_vertex() ) {
		std::for_each( p->particles_in(HepMC::parents).begin(), 
		               p->particles_in(HepMC::parents).end(), 
			       PrintParticle(m_out));
	    }

	    // return immediate children
	    m_out << "\t\t" << "Its children are: " << std::endl;
	    if ( p->end_vertex() ) {
		std::for_each( p->particles_out(HepMC::children).begin(), 
		               p->particles_out(HepMC::children).end(), 
			       PrintChildren(m_out));
	    }

	    // return all descendants
	    // we do this by pointing to the end vertex of the W 
	    // particle and asking for all particle descendants of that vertex
	    m_out << "\t\t Its descendants are: " << std::endl;
	    if ( p->end_vertex() ) {
		std::for_each( p->particles_out(HepMC::descendants).begin(), 
		               p->particles_out(HepMC::descendants).end(), 
			       PrintDescendants(m_out));
	    }
	}	// if IsWBoson
    }
private:
   std::ostream & m_out;
   int            m_event_num;
};

int main() { 
    // declare an input strategy to read the data produced with the 
    // example_MyPythia
    HepMC::IO_GenEvent ascii_in("./testIOGenEvent.input",std::ios::in);
    // declare an instance of the event selection predicate
    IsGoodEvent is_good_event;
    // define some output streams
    std::ofstream os( "testHepMCIteration.out" );
    std::ofstream os2( "testHepMCIteration2.out" );
    std::ofstream os3( "testHepMCIteration3.out" );
    //........................................EVENT LOOP
    int icount=0;
    int num_good_events=0;
    HepMC::GenEvent* evt = ascii_in.read_next_event();
    HepMC::GenEvent* evcopy;
    while ( evt ) {
	icount++;
	if ( icount%50==1 ) std::cout << "Processing Event Number " << icount
				      << " its # " << evt->event_number() 
				      << std::endl;
	// icount of 100 should be the last event
	if ( icount==100 ) std::cout << "Processing Event Number " << icount
				      << " its # " << evt->event_number() 
				      << std::endl;
        evcopy = evt;
	if ( is_good_event(evcopy) ) {
	    ++num_good_events;
	    // simple iteration several different ways
	    os << "Event " << evcopy->event_number() << " is good " << std::endl;
	    simpleIter( evcopy, os );
	    os2 << "Event " << evcopy->event_number() << " is good " << std::endl;
	    simpleIter2( evcopy, os2 );
	    os3 << "Event " << evcopy->event_number() << " is good " << std::endl;
	    simpleIter2( evcopy, os3 );
	    std::cout << "Event " << evcopy->event_number() << " is good " << std::endl;
	    simpleIter3( evcopy );
	    simpleIter4( evcopy );
	    // test iterators
            findW( evcopy, os );
	    // this is the same as findW except that we use the STL for_each algorithm
	    std::for_each( evt->particles_begin(), evt->particles_end(), 
	                   PrintW(os2,evcopy->event_number()));
	    // repeat, using the const iterator
	    std::for_each( evt->particles_begin(), evt->particles_end(), 
	                   PrintConstW(os3,evcopy->event_number()));
	}
	evcopy->clear();
	
	// clean up and get next event
	delete evt;
	evt = ascii_in.read_next_event();
    }
    //........................................PRINT RESULT
    std::cout << num_good_events << " out of " << icount 
	      << " processed events passed the cuts. Finished." << std::endl;
}

bool simpleIter( HepMC::GenEvent* evt, std::ostream& os )
{
    // use GenEvent::vertex_iterator to fill a list of all 
    // vertices in the event
    std::list<HepMC::GenVertex*> allvertices;
    for ( HepMC::GenEvent::vertex_iterator v = evt->vertices_begin();
	  v != evt->vertices_end(); ++v ) {
	allvertices.push_back(*v);
    }

    // fill a list of all final state particles in the event, by requiring
    // that each particle satisfyies the IsFinalState predicate
    IsFinalState isfinal;
    std::list<HepMC::GenParticle*> finalstateparticles;
    for ( HepMC::GenEvent::particle_iterator p = evt->particles_begin();
	  p != evt->particles_end(); ++p ) {
	if ( isfinal(*p) ) finalstateparticles.push_back(*p);
    }

    // print all photons in the event that satisfy the IsPhoton criteria
    os << "photons in event " << evt->event_number() << ":" << std::endl;
    for ( HepMC::GenEvent::particle_iterator p = evt->particles_begin();
	  p != evt->particles_end(); ++p ) {
	if ( IsPhoton(*p) ) (*p)->print( os );
    }

    return true;
}

bool simpleIter2( HepMC::GenEvent* evt, std::ostream& os )
{
    // illustrates the use various helpful algorithms

    // use the STL copy algorithm to fill a list of all 
    // vertices in the event
    std::list<HepMC::GenVertex*> allvertices2;
    copy( evt->vertices_begin(), evt->vertices_end(), 
	  back_inserter(allvertices2) );

    // fill a list of all final state particles in the event, by requiring
    // that each particle satisfyies the IsFinalState predicate
    // an STL-like algorithm called HepMC::copy_if is provided in the
    // GenEvent.h header to do this sort of operation more easily
    std::list<HepMC::GenParticle*> finalstateparticles2;
    HepMC::copy_if( evt->particles_begin(), evt->particles_end(), 
		    back_inserter(finalstateparticles2), IsFinalState() );

    // use the STL for_each algorithm to  
    // print all photons in the event that satisfy the IsPhoton criteria
    os << "photons in event " << evt->event_number() << ":" << std::endl;
    std::for_each(evt->particles_begin(), evt->particles_end(),
	          PrintPhoton(os));

    return true;
}

bool simpleIter3( HepMC::GenEvent* evt, std::ostream& os )
{
    // very simple illustration of using GenEventVertexRange 
    // and GenEventParticleRange
    // NOTE that instead of creating this list, 
    // you can just use GenEventVertexRange as if it were the list
    std::list<HepMC::GenVertex*> allvertices;
    HepMC::GenEventVertexRange vc(*evt);
    for ( HepMC::GenEvent::vertex_iterator v = vc.begin(); v != vc.end(); ++v ) {
	allvertices.push_back(*v);
    }

    // fill a list of all final state particles in the event, by requiring
    // that each particle satisfyies the IsFinalState predicate
    IsFinalState isfinal;
    std::list<HepMC::GenParticle*> finalstateparticles;
    HepMC::GenEventParticleRange pc(*evt);
    for ( HepMC::GenEvent::particle_iterator p = pc.begin(); p != pc.end(); ++p ) {
	if ( isfinal(*p) ) finalstateparticles.push_back(*p);
    }

    // print all photons in the event that satisfy the IsPhoton criteria
    os << "photons in event " << evt->event_number() << ":" << std::endl;
    std::for_each(pc.begin(), pc.end(), PrintPhoton(os));

    return true;
}

bool simpleIter4( HepMC::GenEvent* evt, std::ostream& os )
{
    // very simple illustration of using 
    // GenEvent::vertex_range(), which returns GenEventVertexRange, 
    // and GenEvent::particle_range(), which returns GenEventParticleRange
    // NOTE that instead of creating these lists, 
    // you can just use GenEvent::vertex_range() and GenEvent::particle_range()
    // as if they were a list

    std::list<HepMC::GenVertex*> allvertices;
    for ( HepMC::GenEvent::vertex_iterator v = evt->vertex_range().begin(); 
          v != evt->vertex_range().end(); ++v ) {
	allvertices.push_back(*v);
    }

    // fill a list of all final state particles in the event, by requiring
    // that each particle satisfyies the IsFinalState predicate
    IsFinalState isfinal;
    std::list<HepMC::GenParticle*> finalstateparticles;
    for ( HepMC::GenEvent::particle_iterator p = evt->particle_range().begin(); 
          p != evt->particle_range().end(); ++p ) {
	if ( isfinal(*p) ) finalstateparticles.push_back(*p);
    }

    // print all photons in the event that satisfy the IsPhoton criteria
    os << "photons in event " << evt->event_number() << ":" << std::endl;
    std::for_each(evt->particle_range().begin(), 
                  evt->particle_range().end(), 
		  PrintPhoton(os));

    return true;
}

bool findW( HepMC::GenEvent* evt, std::ofstream& os )
{
    int num_W = 0;
    // use GenEvent::particle_iterator to find all W's in the event,
    // then 
    // (1) for each W user the GenVertex::particle_iterator with a range of
    //     parents to return and print the immediate mothers of these W's.
    // (2) for each W user the GenVertex::particle_iterator with a range of
    //     descendants to return and print all descendants of these W's.
    for ( HepMC::GenEvent::particle_iterator p = evt->particles_begin();
	  p != evt->particles_end(); ++p ) {
	if ( IsWBoson(*p) ) {
	    ++num_W;
	    os << std::endl;
	    os << "A W boson has been found in event: " << evt->event_number() << std::endl;
	    (*p)->print( os );
	    // return all parents
	    // we do this by pointing to the production vertex of the W 
	    // particle and asking for all particle parents of that vertex
	    os << "\t Its parents are: " << std::endl;
	    if ( (*p)->production_vertex() ) {
		for ( HepMC::GenVertex::particle_iterator mother 
			  = (*p)->production_vertex()->
			  particles_begin(HepMC::parents);
		      mother != (*p)->production_vertex()->
			  particles_end(HepMC::parents); 
		      ++mother ) {
		    os << "\t";
		    (*mother)->print( os );
		}
	    }

	    // return immediate children
	    os << "\t\t" << "Its children are: " << std::endl;
	    if ( (*p)->end_vertex() ) {
		for ( HepMC::GenVertex::particle_iterator child = 
        	      (*p)->end_vertex()->particles_begin(HepMC::children); 
		      child != (*p)->end_vertex()->particles_end(HepMC::children); 
		      ++child ) {
		    // make a copy
		    HepMC::GenVertex::particle_iterator cp = child;
		    // use the copy and the original
		    os << "\t\t\t (id,barcode,status) " 
		       << (*cp)->pdg_id() << " " 
        	       << (*child)->barcode() << " "
        	       << (*cp)->status() << std::endl;
		}
	    }

	    // return all descendants
	    // we do this by pointing to the end vertex of the W 
	    // particle and asking for all particle descendants of that vertex
	    os << "\t\t Its descendants are: " << std::endl;
	    if ( (*p)->end_vertex() ) {
		for ( HepMC::GenVertex::particle_iterator des 
			  =(*p)->end_vertex()->
			  particles_begin(HepMC::descendants);
		      des != (*p)->end_vertex()->
			  particles_end(HepMC::descendants);
		      ++des ) {
		    os << "\t\t";
		    (*des)->print( os );
		}
	    }
	}	// if IsWBoson
    }	// end particle loop
    return true;
}
