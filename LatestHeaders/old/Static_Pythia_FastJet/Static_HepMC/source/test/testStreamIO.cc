//////////////////////////////////////////////////////////////////////////
// testStreamIO.cc.in
//
// garren@fnal.gov, March 2006
// 
// The same as testHepMC, but using the IO stream directly
//////////////////////////////////////////////////////////////////////////
//

#include <fstream>

#include "HepMC/GenEvent.h"
#include "HepMC/IO_AsciiParticles.h"
#ifdef HEPMC_HAS_IO_GENEVENT
#include "HepMC/IO_GenEvent.h"
#endif
#include "HepMC/Version.h"
#include "HepMC/IO_Exception.h"

// define methods and classes used by this test
#include "IsGoodEvent.h"
#include "testHepMCMethods.h"

void read_testIOGenEvent(std::ostream & os);
void read_variousFormats(std::ostream & os);
void write_to_stream(std::ostream & os);
void write_to_stream3(std::ostream & os);
void read_from_stream4(std::ostream & os);

int main() { 
    std::ofstream os( "testStreamIO.cout" );
    std::ofstream osv( "testStreamIOVarious.cout" );
    write_to_stream(os);
    read_testIOGenEvent(os);
    read_variousFormats(osv);
    write_to_stream3(os);
    read_from_stream4(os);
    return 0;
}

void write_to_stream(std::ostream & os)
{
    os << std::endl;
    os << "basic IO_GenEvent input with streaming output" << std::endl;
    // declare an input strategy to read the data produced with the 
    // example_MyPythia - units are GeV and mm
    HepMC::IO_GenEvent ascii_in("./testIOGenEvent.input",std::ios::in);
    ascii_in.use_input_units( HepMC::Units::GEV, HepMC::Units::MM );
    // declare an output stream
    const char outfile[] = "testStreamIO.out";
    std::ofstream ascii_out( outfile );
    if( !ascii_out ) {
      std::cerr << "cannot open " << outfile << std::endl;
      exit(-1);
    }
    ascii_out.precision(16);
    HepMC::write_HepMC_IO_block_begin( ascii_out );
    // declare an instance of the event selection predicate
    IsGoodEvent is_good_event;
    //........................................EVENT LOOP
    int icount=0;
    int num_good_events=0;
    HepMC::GenEvent* evt = ascii_in.read_next_event();
    while ( evt ) {
	icount++;
	if ( icount%50==1 ) os << "Processing Event Number " << icount
				      << " its # " << evt->event_number() 
				      << std::endl;
	if ( is_good_event(evt) ) {
	    ++num_good_events;
	    particleTypes( evt, os );
	    ascii_out << (*evt);
	}
	
	// clean up and get next event
	delete evt;
	ascii_in >> evt;
    }
    HepMC::write_HepMC_IO_block_end( ascii_out );
    //........................................PRINT RESULT
    os << num_good_events << " out of " << icount 
	      << " processed events passed the cuts. Finished." << std::endl;
}

void read_testIOGenEvent(std::ostream & os)
{
    os << std::endl;
    os << "streaming input and output" << std::endl;
    // input units are GeV and mm
    const char infile[] = "./testIOGenEvent.input";
    std::ifstream is( infile );
    if( !is ) {
      std::cerr << "cannot open " << infile << std::endl;
      exit(-1);
    }
    // declare an output stream
    const char outfile[] = "testStreamIO2.out";
    std::ofstream ascii_out( outfile );
    if( !ascii_out ) {
      std::cerr << "cannot open " << outfile << std::endl;
      exit(-1);
    }
    ascii_out.precision(16);
    HepMC::write_HepMC_IO_block_begin( ascii_out );
    // declare another output stream to test precision
    const char poutfile[] = "testStreamIOprecision.out";
    std::ofstream pout( poutfile );
    if( !pout ) {
      std::cerr << "cannot open " << poutfile << std::endl;
      exit(-1);
    }
    pout.precision(10);
    // declare an IO_AsciiParticle for output
    HepMC::IO_AsciiParticles particle_out("testStreamIOParticle.out",std::ios::out);
    // declare an instance of the event selection predicate
    IsGoodEvent is_good_event;
    //........................................EVENT LOOP
    int icount=0;
    int num_good_events=0;
    HepMC::GenEvent evt;
    while ( is ) {
	// WARNING - we are not using pointers, so this could be an empty event
        is >> evt;
	// make sure this is a valid event
	if( evt.is_valid() ) {
	    ++icount;
	    if ( icount%50==1 ) os << "Processing Event Number " << icount
					  << " its # " << evt.event_number() 
					  << std::endl;
	    if ( is_good_event( &evt ) ) {
		++num_good_events;
		particleTypes(&evt,os);
		ascii_out << evt;
		pout << evt;
		// We must explicitly create the pointer if we want to use this event
		// with any IO strategy (e.g., IO_AsciiParticles)
		HepMC::GenEvent* pevt= &evt;
		particle_out << pevt;
	    }
	}
    }
    HepMC::write_HepMC_IO_block_end( ascii_out );
    //........................................PRINT RESULT
    os << num_good_events << " out of " << icount 
	      << " processed events passed the cuts. Finished." << std::endl;
}

void read_variousFormats(std::ostream & os)
{
    os << std::endl;
    os << "process varied input" << std::endl;
    // declare an input stream 
    const char infile[] = "./testHepMCVarious.input";
    std::ifstream is( infile );
    if( !is ) {
      std::cerr << "cannot open " << infile << std::endl;
      exit(-1);
    }
    // set input units
    HepMC::set_input_units( is, HepMC::Units::GEV, HepMC::Units::MM );
    // declare an output stream
    const char outfile[] = "testStreamIOVarious.out";
    std::ofstream ascii_out( outfile );
    if( !ascii_out ) {
      std::cerr << "cannot open " << outfile << std::endl;
      exit(-1);
    }
    ascii_out.precision(16);
    HepMC::write_HepMC_IO_block_begin( ascii_out );
    //........................................EVENT LOOP
    int icount=0, ibad=0;
    HepMC::GenEvent evt;
    while ( is ) {
        // we have to do our own try/catch blocks
        try {
           is >> evt;
	}
        catch (HepMC::IO_Exception& e) {
	    evt.clear();
 	    ++ibad;
        }
	// WARNING - we are not using pointers, so this could be an empty event
	// make sure this is a valid event
	if( evt.is_valid() ) {
	    icount++;
	    double pim;
	    os << "Processing Event Number " << icount
		      << " its # " << evt.event_number() 
		      << std::endl;
	    ascii_out << evt;
	    // units should be unknown
	    evt.write_units(os);
	    pim = findPiZero(&evt);
	    os << " pizero mass: " << pim << std::endl;
	    // set units to GeV and mm
	    evt.use_units(HepMC::Units::GEV, HepMC::Units::MM);
	    evt.write_units(os);
	    pim = findPiZero(&evt);
	    os << " pizero mass: " << pim 
	              << " " << HepMC::Units::name( evt.momentum_unit() ) << std::endl;
	    // convert units to MeV
	    evt.use_units(HepMC::Units::MEV, HepMC::Units::MM);
	    evt.write_units(os);
	    pim = findPiZero(&evt);
	    os << " pizero mass: " << pim 
	              << " " << HepMC::Units::name( evt.momentum_unit() ) << std::endl;
        }
    }
    std::cout << "testSteamIO: the HeavyIon and PdfInfo input stream errors are intentional" << std::endl;
    HepMC::write_HepMC_IO_block_end( ascii_out );
    //........................................PRINT RESULT
    os << icount << " valid events processed.  " ;
    os << ibad << " invalid events processed. Finished." << std::endl;
}

void write_to_stream3(std::ostream & os)
{
    os << std::endl;
    os << "basic IO_GenEvent input with streaming output using member function" << std::endl;
    // declare an input strategy to read the data produced with the 
    // example_MyPythia - units are GeV and mm
    HepMC::IO_GenEvent ascii_in("./testIOGenEvent.input",std::ios::in);
    ascii_in.use_input_units( HepMC::Units::GEV, HepMC::Units::MM );
    // declare an output stream
    const char outfile[] = "testStreamIO3.out";
    std::ofstream ascii_out( outfile );
    if( !ascii_out ) {
      std::cerr << "cannot open " << outfile << std::endl;
      exit(-1);
    }
    ascii_out.precision(16);
    HepMC::write_HepMC_IO_block_begin( ascii_out );
    // declare an instance of the event selection predicate
    IsGoodEvent is_good_event;
    //........................................EVENT LOOP
    int icount=0;
    int num_good_events=0;
    HepMC::GenEvent* evt = ascii_in.read_next_event();
    while ( evt ) {
	icount++;
	if ( icount%50==1 ) os << "Processing Event Number " << icount
				      << " its # " << evt->event_number() 
				      << std::endl;
	if ( is_good_event(evt) ) {
	    ++num_good_events;
	    particleTypes( evt, os );
	    evt->write(ascii_out);
	}
	
	// clean up and get next event
	delete evt;
	ascii_in >> evt;
    }
    HepMC::write_HepMC_IO_block_end( ascii_out );
    //........................................PRINT RESULT
    os << num_good_events << " out of " << icount 
	      << " processed events passed the cuts. Finished." << std::endl;
}

void read_from_stream4(std::ostream & os)
{
    os << std::endl;
    os << "streaming input and output using member functions" << std::endl;
    // input units are GeV and mm
    const char infile[] = "./testIOGenEvent.input";
    std::ifstream is( infile );
    if( !is ) {
      std::cerr << "cannot open " << infile << std::endl;
      exit(-1);
    }
    // declare an output stream
    const char outfile[] = "testStreamIO4.out";
    std::ofstream ascii_out( outfile );
    if( !ascii_out ) {
      std::cerr << "cannot open " << outfile << std::endl;
      exit(-1);
    }
    ascii_out.precision(16);
    HepMC::write_HepMC_IO_block_begin( ascii_out );
    // declare an instance of the event selection predicate
    IsGoodEvent is_good_event;
    //........................................EVENT LOOP
    int icount=0;
    int num_good_events=0;
    HepMC::GenEvent evt;
    while ( is ) {
	// WARNING - we are not using pointers, so this could be an empty event
        evt.read(is);
	// make sure this is a valid event
	if( evt.is_valid() ) {
	    ++icount;
	    if ( icount%50==1 ) os << "Processing Event Number " << icount
					  << " its # " << evt.event_number() 
					  << std::endl;
	    if ( is_good_event( &evt ) ) {
		++num_good_events;
		particleTypes(&evt,os);
		evt.write(ascii_out);
	    }
	}
    }
    HepMC::write_HepMC_IO_block_end( ascii_out );
    //........................................PRINT RESULT
    os << num_good_events << " out of " << icount 
	      << " processed events passed the cuts. Finished." << std::endl;
}
