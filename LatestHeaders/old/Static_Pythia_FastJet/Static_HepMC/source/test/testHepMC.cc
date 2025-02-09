//-------------------------------------------------------------------
// testHepMC.cc.in
//
// garren@fnal.gov, March 2006
// based on example_EventSelection
// Apply an event selection to the events in testHepMC.input
// Events containing a photon of pT > 25 GeV pass the selection
// and are written to "testHepMC.out"
// Also write events using IO_AsciiParticles 
//-------------------------------------------------------------------
//

#include "HepMC/GenEvent.h"
#include "HepMC/GenCrossSection.h"
#ifndef HEPMC_IO_ASCII_REMOVED
#include "HepMC/IO_Ascii.h"
#endif
#ifdef HEPMC_HAS_IO_GENEVENT
#include "HepMC/IO_GenEvent.h"
#endif
#include "HepMC/IO_AsciiParticles.h"

// define methods and classes used by this test
#include "IsGoodEvent.h"
#include "testHepMCMethods.h"

void read_testIOGenEvent(std::ostream & os);
void read_testUnits(std::ostream & os);
void read_variousFormats(std::ostream & os);
void writeWithCrossSection(std::ostream & os);
void readWithCrossSection(std::ostream & os);
void writeWithWeight(std::ostream & os);
void readWithWeight(std::ostream & os);
void read_nan(std::ostream & os);

int main() { 
    std::ofstream os( "testHepMC.cout" );
    std::ofstream osv( "testHepMCVarious.cout" );
    read_testIOGenEvent(os);
    read_testUnits(os);
    read_variousFormats(osv);
    read_nan(os);
    writeWithCrossSection(os);
    readWithCrossSection(os);
    writeWithWeight(os);
    readWithWeight(os);
    return 0;
}

void read_testIOGenEvent(std::ostream & os)
{
    os << std::endl;
    os << "basic IO_GenEvent input and output" << std::endl;
    // declare an input strategy to read the data produced with the 
    // example_MyPythia - units are GeV and mm
    HepMC::IO_GenEvent ascii_in("./testIOGenEvent.input",std::ios::in);
    ascii_in.use_input_units( HepMC::Units::GEV, HepMC::Units::MM );
    // declare another IO_GenEvent for writing out the good events
    HepMC::IO_GenEvent ascii_out("testHepMC.out",std::ios::out);
    // declare an output IO_GenEvent for testing precision
    HepMC::IO_GenEvent prec_out("testHepMCprecision.out",std::ios::out);
    prec_out.precision(10);
    // declare an IO_AsciiParticle for output
    HepMC::IO_AsciiParticles particle_out("testHepMCParticle.out",std::ios::out);
    // declare an instance of the event selection predicate
    IsGoodEvent is_good_event;
    //........................................EVENT LOOP
    int icount=0;
    int num_good_events=0;
    HepMC::GenEvent* evt = ascii_in.read_next_event();
    while ( evt ) {
	++icount;
	if ( icount%50==1 ) os << "Processing Event Number " << icount
				      << " its # " << evt->event_number() 
				      << std::endl;
	if ( is_good_event(evt) ) {
	    particleTypes(evt,os);
	    // verify use_input_units()
	    evt->write_units(os);
	    double pim = findPiZero(evt);
	    os << " pizero mass: " << pim << std::endl;
	    //
	    ascii_out << evt;
	    particle_out << evt;
	    prec_out << evt;
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

void read_testUnits(std::ostream & os)
{
    os << std::endl;
    os << "IO_GenEvent input and output using define_units" << std::endl;
    // declare an input strategy to read the data produced with the 
    // example_MyPythia - units are GeV and mm
    // we DO NOT define input units here, instead we use define_units
    HepMC::IO_GenEvent ascii_in("./testIOGenEvent.input",std::ios::in);
    // declare another IO_GenEvent for writing out the good events
    HepMC::IO_GenEvent ascii_out("testDefineUnits.out",std::ios::out);
    // declare an instance of the event selection predicate
    IsGoodEvent is_good_event;
    //........................................EVENT LOOP
    int icount=0;
    int num_good_events=0;
    HepMC::GenEvent* evt = ascii_in.read_next_event();
    while ( evt ) {
	++icount;
	evt->define_units( HepMC::Units::GEV, HepMC::Units::MM );
	if ( icount%50==1 ) os << "Processing Event Number " << icount
				      << " its # " << evt->event_number() 
				      << std::endl;
	if ( is_good_event(evt) ) {
	    // verify define_units()
	    evt->write_units(os);
	    double pim = findPiZero(evt);
	    os << " pizero mass: " << pim << std::endl;
	    //
	    particleTypes(evt,os);
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

void read_variousFormats(std::ostream & os)
{
    os << std::endl;
    os << "process varied input" << std::endl;
    // declare an input strategy 
    HepMC::IO_GenEvent ascii_in("./testHepMCVarious.input",std::ios::in);
    ascii_in.use_input_units( HepMC::Units::GEV, HepMC::Units::MM );
    // declare another IO_GenEvent for writing out the good events
    HepMC::IO_GenEvent ascii_out("testHepMCVarious.out",std::ios::out);
    //........................................EVENT LOOP
    int icount=0;
    HepMC::GenEvent* evt = ascii_in.read_next_event();
    while ( evt ) {
	icount++;
	double pim;
	os << "Processing Event Number " << icount
		  << " its # " << evt->event_number() 
		  << std::endl;
	ascii_out << evt;
	// units should be unknown
	evt->write_units(os);
	pim = findPiZero(evt);
	os << " pizero mass: " << pim << std::endl;
	if( HepMC::Units::name( evt->momentum_unit() ) == "GEV" ) {
	    os << " GenEvent units are GeV" << std::endl;
	    if( pim > 1.0 ) {
		// presume units are MEV and out of sync
		os << " pizero units are MeV" << std::endl;
		repairUnits(evt,HepMC::Units::MEV,HepMC::Units::GEV);
		// set units to MeV and mm
		evt->use_units(HepMC::Units::MEV, HepMC::Units::MM);
		evt->write_units(os);
		pim = findPiZero(evt);
		os << " pizero mass: " << pim 
	        	  << " " << HepMC::Units::name( evt->momentum_unit() ) << std::endl;
		// convert units to MeV
		evt->use_units(HepMC::Units::MEV, HepMC::Units::MM);
		evt->write_units(os);
		pim = findPiZero(evt);
		os << " pizero mass: " << pim 
	        	  << " " << HepMC::Units::name( evt->momentum_unit() ) << std::endl;
	    } else if( pim > 0.1 ) {
		// presume units are GEV
		os << " pizero units are GeV" << std::endl;
		// set units to GeV and mm
		evt->use_units(HepMC::Units::GEV, HepMC::Units::MM);
		evt->write_units(os);
		pim = findPiZero(evt);
		os << " pizero mass: " << pim 
	        	  << " " << HepMC::Units::name( evt->momentum_unit() ) << std::endl;
		// convert units to MeV
		evt->use_units(HepMC::Units::MEV, HepMC::Units::MM);
		evt->write_units(os);
		pim = findPiZero(evt);
		os << " pizero mass: " << pim 
	        	  << " " << HepMC::Units::name( evt->momentum_unit() ) << std::endl;
	    } else {
		os << " pizero mass: " << pim 
	        	  << " is inconsistent with allowed units " << std::endl;
	    }
	} else if( HepMC::Units::name( evt->momentum_unit() ) == "MEV" ) {
	    os << " GenEvent units are MeV" << std::endl;
	    if( pim > 1.0 ) {
		// presume units are MEV
		os << " pizero units are MeV" << std::endl;
		// set units to MeV and mm
		evt->use_units(HepMC::Units::MEV, HepMC::Units::MM);
		evt->write_units(os);
		pim = findPiZero(evt);
		os << " pizero mass: " << pim 
	        	  << " " << HepMC::Units::name( evt->momentum_unit() ) << std::endl;
		// convert units to MeV
		evt->use_units(HepMC::Units::MEV, HepMC::Units::MM);
		evt->write_units(os);
		pim = findPiZero(evt);
		os << " pizero mass: " << pim 
	        	  << " " << HepMC::Units::name( evt->momentum_unit() ) << std::endl;
	    } else if( pim > 0.1 ) {
		// presume units are GEV and out of sync
		os << " pizero units are GeV" << std::endl;
		repairUnits(evt,HepMC::Units::GEV,HepMC::Units::MEV);
		evt->write_units(os);
		pim = findPiZero(evt);
		os << " pizero mass: " << pim 
	        	  << " " << HepMC::Units::name( evt->momentum_unit() ) << std::endl;
		// convert units to MeV
		evt->use_units(HepMC::Units::MEV, HepMC::Units::MM);
		evt->write_units(os);
		pim = findPiZero(evt);
		os << " pizero mass: " << pim 
	        	  << " " << HepMC::Units::name( evt->momentum_unit() ) << std::endl;
	    } else {
		os << " pizero mass: " << pim 
	        	  << " is inconsistent with allowed units " << std::endl;
	    }
	}
	// clean up and get next event
	delete evt;
	ascii_in >> evt;
    }
    std::cout << "testHepMC: the HeavyIon and PdfInfo input stream errors are intentional" << std::endl;
    //........................................PRINT RESULT
    os << icount << " events processed. Finished." << std::endl;
}

void writeWithCrossSection(std::ostream & os)
{
    // declare an input strategy to read input data 
    // units are GeV and mm
    HepMC::IO_GenEvent ascii_in("./testIOGenEvent.input",std::ios::in);
    ascii_in.use_input_units( HepMC::Units::GEV, HepMC::Units::MM );
    // declare another IO_GenEvent for writing out some events
    HepMC::IO_GenEvent ascii_out("testCrossSection.out",std::ios::out);
    // declare an output stream for printing events
    std::ofstream xout( "testCrossSection.cout" );
    // create an empty GenCrossSection object
    HepMC::GenCrossSection cross;
    //........................................EVENT LOOP
    int icount=0;
    const double xs0 = 0.00346;
    const double xs1 = 0.12;
    const double xs2 = 33.234;
    const double xs3 = 459.345;
    double xserr = 0.0001;
    double wgt1, wgt2;
    HepMC::GenEvent* evt = ascii_in.read_next_event();
    while ( evt ) {
	icount++;
	// use a variety of arbitrary cross section values
	if( icount < 10 ) {
	    const double xs = xs0 - 1.34 * xserr;
	    cross.set_cross_section( xs, xserr );
	} else if( icount < 20 ) {
	    const double xs = xs1 - 1.34 * xserr;
	    cross.set_cross_section( xs, xserr );
	} else if( icount < 30 ) {
	    const double xs = xs2 - 1.34 * xserr;
	    cross.set_cross_section( xs, xserr );
	} else {
	    const double xs = xs3 - 1.34 * xserr;
	    cross.set_cross_section( xs, xserr );
	}
	xserr *= 0.99;
	if ( icount == 10 ) xserr += 0.01;
	if ( icount == 20 ) xserr += 0.4;
	if ( icount == 30 ) xserr += 1.0;
	// attach this cross section to the event
	evt->set_cross_section( cross );
	evt->write_cross_section(os);
	// add weights
	wgt1 = 0.9853 + (double)icount * 0.00033;
	wgt2 = 0.9853 + (double)(icount+1) * 0.00033;
	evt->weights().push_back(0.3456);
	evt->weights()["weightName"] = wgt1;
	evt->weights()["second weight name"] = wgt2;
	if ( icount%20==1 ) {
	    os << "writeWithCrossSection: Processing Event Number " << icount
				      << " its # " << evt->event_number() 
				      << std::endl;
	    ascii_out << evt;
	    evt->print(xout);
	}
	
	// clean up and get next event
	delete evt;
	ascii_in >> evt;
    }
    //........................................PRINT RESULT
    os << "writeWithCrossSection processed " << icount << " events. Finished." << std::endl;
}

void readWithCrossSection(std::ostream & os)
{
    // read the file we just wrote
    HepMC::IO_GenEvent ascii_in("testCrossSection.out",std::ios::in);
    // declare another IO_GenEvent for writing out some events
    HepMC::IO_GenEvent ascii_out("testCrossSection2.out",std::ios::out);
    //........................................EVENT LOOP
    int icount=0;
    HepMC::GenEvent* evt = ascii_in.read_next_event();
    while ( evt ) {
	++icount;
	os << "readWithCrossSection: Processing Event Number " << icount
				  << " its # " << evt->event_number() 
				  << std::endl;
	if (evt->cross_section()->cross_section() <= 0) {
          os << "testReadCrossSection: invalid cross-section!" << std::endl;
	}
	ascii_out << evt;
	
	// clean up and get next event
	delete evt;
	ascii_in >> evt;
    }
    //........................................PRINT RESULT
    os << "readWithCrossSection processed " << icount << " events. Finished." << std::endl;
}

void read_nan(std::ostream & os)
{
    // Read an input file that has corrupt information (nan's)
    //
    HepMC::IO_GenEvent xin("./testHepMCVarious.input",std::ios::in);
    HepMC::IO_GenEvent xout("testNaN.out",std::ios::out);
    // set input units
    xin.use_input_units( HepMC::Units::GEV, HepMC::Units::MM );
    //........................................EVENT LOOP
    int icount=0;
    int invaliddata=0;
    bool ok = true;
    os << "---------------------------------------- " << std::endl;
    os << "Begin NaN test " << std::endl;
    HepMC::GenEvent* evt = xin.read_next_event();
    //
    // To recover from corrupt input, replace "while(evt) {...}" 
    // with  "while(ok) { if(evt) {... xin >> evt;} else {...} }"
    //
    while ( ok ) {
	if( evt ) {
	    ++icount;
	    os << "read_nan: Processing Event Number " << icount
		      << " its # " << evt->event_number() 
		      << std::endl;
	    xout << evt;
	    // clean up and get next event
	    delete evt;
	    xin >> evt;
	} else if (xin.error_type() == HepMC::IO_Exception::InvalidData ) {
	    ++invaliddata;
            os << "INPUT ERROR: " << xin.error_message() << std::endl;
	    // clean up and get next event
	    delete evt;
	    xin >> evt;
	} else if (invaliddata > 50 ) {
            os << "INPUT ERROR: " << xin.error_message() << std::endl;
	    ok = false;
	} else {
	    ok = false;
	}
    }
    // print status of input stream
    if ( xin.error_type() != 0 ) {
        os << "processing of ./testHepMCVarious.input ended with error " 
	          << xin.error_type() << std::endl;
        os << "  --- " << xin.error_message() << std::endl;
    }
    os << icount << " events processed and " 
              << invaliddata << " events ignored. Finished." 
              << std::endl;
    os << "End NaN test " << std::endl;
    os << "---------------------------------------- " << std::endl;
}

void writeWithWeight(std::ostream & os)
{
    // declare an input strategy to read input data 
    // units are GeV and mm
    HepMC::IO_GenEvent ascii_in("./testIOGenEvent.input",std::ios::in);
    ascii_in.use_input_units( HepMC::Units::GEV, HepMC::Units::MM );
    // declare another IO_GenEvent for writing out some events
    HepMC::IO_GenEvent ascii_out("testWithWeight.out",std::ios::out);
    // declare an output stream for printing events
    std::ofstream xout( "testWithWeight.cout" );
    //........................................EVENT LOOP
    int icount=0;
    double wgt1, wgt2;
    HepMC::GenEvent* evt = ascii_in.read_next_event();
    while ( evt ) {
	icount++;
	// add weights
	wgt1 = 0.9853 + (double)icount * 0.00033;
	wgt2 = 0.9853 + (double)(icount+1) * 0.00033;
	evt->weights().push_back(0.3456);
	evt->weights().push_back(wgt1);
	evt->weights().push_back(wgt2);
	if ( icount%20==1 ) {
	    os << "writeWithWeight: Processing Event Number " << icount
				      << " its # " << evt->event_number() 
				      << std::endl;
	    ascii_out << evt;
	    evt->print(xout);
	}
	
	// clean up and get next event
	delete evt;
	ascii_in >> evt;
    }
    //........................................PRINT RESULT
    os << "writeWithWeight processed " << icount << " events. Finished." << std::endl;
}

void readWithWeight(std::ostream & os)
{
    // read the file we just wrote
    HepMC::IO_GenEvent ascii_in("testWithWeight.out",std::ios::in);
    // declare another IO_GenEvent for writing out some events
    HepMC::IO_GenEvent ascii_out("testWithWeight2.out",std::ios::out);
    //........................................EVENT LOOP
    int icount=0;
    HepMC::GenEvent* evt = ascii_in.read_next_event();
    while ( evt ) {
	++icount;
	os << "readWithWeight: Processing Event Number " << icount
				  << " its # " << evt->event_number() 
				  << std::endl;
	if ( !evt->cross_section() ) {
          os << "testReadCrossSection: invalid cross-section!" << std::endl;
	}
	ascii_out << evt;
	
	// clean up and get next event
	delete evt;
	ascii_in >> evt;
    }
    //........................................PRINT RESULT
    os << "readWithWeight processed " << icount << " events. Finished." << std::endl;
}
