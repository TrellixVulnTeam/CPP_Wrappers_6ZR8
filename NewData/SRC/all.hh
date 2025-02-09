#define USE_ALL
#include "./LatestHeaders/NewHEPHeaders/main.hh"
#include "./HEPTopTagger/main.hh"
#include "./BadImageFormer.hh"
////////////////////////////////////////////////////////////////
namespace MISC {
    //
	size_t constexpr
		ImageResolution =
			40
	; //

	using TYPE_DATA =
		float
	; //

	using UnBoostedImageType =
		NoBoost::BoxImageGen <
			ImageResolution	,
			TYPE_DATA		,
			false
		>
	; //

	using NoGramSchmidtImageType =
		NoGramSchmidt::BoxImageGen <
			ImageResolution	,
			TYPE_DATA		,
			false
		>
	; //

	using NoGramSchmidtBADImageType =
		NoGramSchmidt_BAD::BoxImageGen <
			ImageResolution	,
			TYPE_DATA		,
			false
		>
	; //

	using imagetype =
		NewHEPHeaders::BoxImageGen <
			ImageResolution	,
			TYPE_DATA
		>
	; //

	using imagetypeflip =
		NewHEPHeaders::ImageGenFlip <
			ImageResolution	,
			TYPE_DATA
		>
	; //

    using outvector4 =
        NewHEPHeaders::VECTORS::lorentz4vector
            <TYPE_DATA>
    ; //
    using TYPE_NSub =
        Tensors::Array::ND_ARRAY
            <5,TYPE_DATA>
    ; //
    using TYPE_PREDICT =
        Tensors::Array::ND_ARRAY <
            ImageResolution*ImageResolution,
            TYPE_DATA
        >
    ; //
    using vector4  =
        fastjet::PseudoJet
    ; //
    using vector4s =
        std::vector
            <fastjet::PseudoJet>
    ; //
    using READER_NSub =
		CPPFileIO::FullFileReader
		<TYPE_NSub>
	; //

	using READER_Loss =
		CPPFileIO::FullFileReader
		<TYPE_DATA>
	; //

	using READER_Vector =
		CPPFileIO::FullFileReader
		<outvector4>
	; //
	using names =
		std::vector
			<std::string>
	; //
    //
    inline void
    Executor (
        std::string
            ProgramName
    ) {
        CPPFileIO::ForkMe forker ;
        if(forker.InKid()){
            std::vector
                <std::string>
                    program
            ; //
            program.push_back
                (ProgramName)
            ; //
            CPPFileIO::starter_self
                (program)
            ; //
        }
    }
    //
    class MyPythia :
    public Pythia8::Pythia {
    public:
        //
        inline long
        FindParticle (
            long pid
        ) {
            long ret = 0 ;
            for (
                long i = 0 ;
                (i<this->event.size()) && (ret==0) ;
                i++
            ) if (
                this->event[i].id() == pid
            ) { ret = i ; }
            return ret ;
        }
        //
        inline long
        FindParticleMod (
            long pid
        ) {
            long ret = 0 ;
            pid = CPPFileIO::mymod (pid) ;
            for (
                long i=0;
                (i<this->event.size())&&(ret==0);
                i++
            ) if (
                CPPFileIO::mymod(
                    this->event[i].id()
                ) == pid
            ) { ret = i ; }
            return ret ;
        }
        //
        inline long
        Recurse (
            long index
        ) {
            auto ppid =
                this->event[index].id ()
            ; //
            long dt[2] = {
                    this->event[index]
                        .daughter1 () ,
                    this->event[index]
                        .daughter2 ()
                }
            ; //
            long dpid[2] = {
                    this->event[dt[0]]
                        .id () ,
                    this->event[dt[1]]
                        .id ()
                }
            ; //
            if (dpid[0]==ppid) {
                return
                    Recurse (dt[0])
                ;
            }
            else if (dpid[1]==ppid) {
                return
                    Recurse (dt[1])
                ;
            }
            else {
                return
                    index
                ;
            }
        }
        //
        inline long
        FindDaughter (
            long ParentID ,
            long DPID
        ) {
            ParentID =
                Recurse (ParentID)
            ; //
            long dt[2] = {
                    this->event[ParentID]
                        .daughter1() ,
                    this->event[ParentID]
                        .daughter2()
                }
            ; //
            long dpid[2] = {
                    this->event[dt[0]]
                        .id() ,
                    this->event[dt[1]]
                        .id()
                }
            ; //
            if (dpid[0]==DPID) {
                return
                    dt[0]
                ;
            }
            else if (dpid[1]==DPID) {
                return
                    dt[1]
                ;
            }
            else {
                return
                    0
                ;
            }
        }
        //
        inline vector4
        GetVector (
            long id
        ) {
            return
                vector4 (
                    this->event[id].px () ,
                    this->event[id].py () ,
                    this->event[id].pz () ,
                    this->event[id].e  ()
                )
            ;
        }
        //
    } ;
    //
    class EventWriter {
    public:
        //
        static inline TYPE_NSub
        EvalNSub (
            vector4 const &
                injet
        ) {
            constexpr double
                beta = 2.0
            ; //
            using namespace std              ;
            using namespace fastjet          ;
            using namespace fastjet::contrib ;
            TYPE_NSub ret ;
            /* TAU 1 */ {
                Nsubjettiness
                    nSub1_beta2 (
                        1 ,
                        OnePass_WTA_KT_Axes() ,
                        UnnormalizedMeasure(beta)
                    )
                ; //
                ret[0] =
                    nSub1_beta2
                        (injet)
                ; //
            }
            /* TAU 2 */ {
                Nsubjettiness
                    nSub2_beta2 (
                        2 ,
                        OnePass_WTA_KT_Axes() ,
                        UnnormalizedMeasure(beta)
                    )
                ; //
                ret[1] =
                    nSub2_beta2
                        (injet)
                ; //
            }
            /* TAU 3 */ {
                Nsubjettiness
                    nSub3_beta2 (
                        3 ,
                        OnePass_WTA_KT_Axes() ,
                        UnnormalizedMeasure(beta)
                    )
                ; //
                ret[2] =
                    nSub3_beta2
                        (injet)
                ; //
            }
            /* TAU 4 */ {
                Nsubjettiness
                    nSub4_beta2 (
                        4 ,
                        OnePass_WTA_KT_Axes() ,
                        UnnormalizedMeasure(beta)
                    )
                ; //
                ret[3] =
                    nSub4_beta2
                        (injet)
                ; //
            }
            /* TAU 5 */ {
                Nsubjettiness
                    nSub5_beta2 (
                        5 ,
                        OnePass_WTA_KT_Axes() ,
                        UnnormalizedMeasure(beta)
                    )
                ; //
                ret[4] =
                    nSub5_beta2
                        (injet)
                ; //
            }
            return ret ;
        }
        //
        static inline bool
        CheckTopTag (
            vector4s const &
                particles
        ) {
            fastjet::JetAlgorithm algo =
                fastjet::cambridge_algorithm
            ; //
            fastjet::JetDefinition
                jet_def (
                    algo,
                    100.0
                )
            ;
            fastjet::ClusterSequence
                clust_seq (
                    particles,
                    jet_def
                )
            ; //
            vector4s jets =
                clust_seq
                .exclusive_jets(1)
            ; //
            HEPTopTagger::HEPTopTagger
                tagger (jets[0])
            ; /* Prepare and run the tagger: */ {
                tagger.set_max_subjet_mass (30.0) ;
                tagger.set_mass_drop_threshold (0.8) ;
                tagger.set_filtering_R (0.3) ;
                tagger.set_filtering_n (5) ;
                tagger.set_filtering_minpt_subjet (100.0) ;
                tagger.set_mode (HEPTopTagger::TWO_STEP_FILTER) ;
                tagger.set_top_minpt (800) ;
                tagger.set_top_mass_range (165.0,185.0) ;
                tagger.set_fw (0.1) ;
                tagger.run();
            }
            return tagger.is_tagged() ;
        }
        //
        inline void
        Eval (
            vector4 const &
                injet
        ) {
            /* Evaluate NSubjettiness  : */ {
                auto tmptau =
                    EvalNSub(injet)
                ;
                NSubWriter.push_back
                    (tmptau)
                ; //
            }
            /* Evaluate Normal Image   : */ {
                imagetype img (
                    injet.constituents() ,
                    0.5
                ) ; //
                ImageWriter.push_back
                    (img)
                ; //
            }
            /* Evaluate UnBoosted Image   : */ {
                UnBoostedImageType img (
                    injet.constituents()
                ) ; //
                UnBoostedImageWriter.push_back
                    (img)
                ; //
            }

            /* Evaluate NoGramSchmidt Image : */ {
                NoGramSchmidtBADImageType img (
                    injet.constituents() ,
                    0.5
                ) ; //
                NoGramSchmidtBADImageWriter.push_back
                    (img)
                ; //
            }

            /* Evaluate NoGramSchmidt Image : */ {
                NoGramSchmidtImageType img (
                    injet.constituents() ,
                    0.5
                ) ; //
                NoGramSchmidtImageWriter.push_back
                    (img)
                ; //
            }
            /* Write Top Tagged bool   : */ {
                TopTagWriter.push_back(
                    CheckTopTag (
                        injet.constituents()
                    )
                ) ; //
            }
            /* Write actual jet vector : */ {
                outvector4 tmp (
                    injet
                ) ;
                VectorWriter.push_back
                    (tmp)
                ; //
            }
        }
        //
        inline void
        operator () (
            vector4 const &
                injet
        ) { Eval (injet) ; }
        //
        std::string prefixname ;

        CPPFileIO::FileVector	< imagetype					>
            ImageWriter
        ; //
        CPPFileIO::FileVector	< UnBoostedImageType		>
            UnBoostedImageWriter
        ; //
        CPPFileIO::FileVector	< NoGramSchmidtImageType	>
            NoGramSchmidtImageWriter
        ; //
        CPPFileIO::FileVector	< NoGramSchmidtBADImageType	>
            NoGramSchmidtBADImageWriter
        ; //
        CPPFileIO::FileVector	< outvector4				>
            VectorWriter
        ; //
        CPPFileIO::FileVector	< bool						>
            TopTagWriter
        ; //
        CPPFileIO::FileVector	< TYPE_NSub					>
            NSubWriter
        ; //

        EventWriter					( std::string _prefixname					) :
        prefixname					( _prefixname								) ,
        ImageWriter					( prefixname + "/image"						) ,
        UnBoostedImageWriter		( prefixname + "/UnBoostedimage"			) ,
        NoGramSchmidtImageWriter	( prefixname + "/NoGramSchmidtImageType"	) ,
        NoGramSchmidtBADImageWriter	( prefixname + "/NoGramSchmidtBADImageType"	) ,
        VectorWriter				( prefixname + "/vector"					) ,
        TopTagWriter				( prefixname + "/toptag"					) ,
        NSubWriter					( prefixname + "/nsub"						) {}
        //
        ~EventWriter(){}
        //
    } ;
    //
    template <
        size_t Num,
        bool compare=false
    > class MyHistN {
    private:
        std::string   histname         ;
        std::string   histNnames [Num] ;
        TH1F        * Hists      [Num] ;
        bool logscale ;
    public:
        template <size_t Ii> inline void Fill (double a) { if (a>-90.0) {Hists[Ii]->Fill(a);} }

        inline void Fill (double a,size_t Ii) { if (a>-90.0) {Hists[Ii]->Fill(a);} }

        inline void NormalizeHist (TH1F * inhist) { inhist->Scale(1.0/inhist->Integral()); inhist->SetLineWidth(3); }
        inline void DeleteAll () { for (size_t ii=0;ii<Num;ii++) { delete Hists[ii] ; } }
        inline void Write () {
            //printf("DEBUG: Called Write();");
            TCanvas C ; if(logscale){C.SetLogy();}
            Style_t styles[Num] ; /* Set the line style. */ {for(size_t i=0;i<Num;i++){styles[i]=kSolid;}}
            std::vector <int> Colors ; /* Decide Colors: */ {
                if ((Num==6)&&compare) {
                    styles[0] = kSolid  ;
                    styles[1] = kSolid  ;
                    styles[2] = kSolid  ;
                    styles[3] = kDashed ;
                    styles[4] = kDashed ;
                    styles[5] = kDashed ;
                    Colors.push_back(TColor::GetColor("#990000"));
                    Colors.push_back(TColor::GetColor("#009900"));
                    Colors.push_back(TColor::GetColor("#000099"));
                    Colors.push_back(TColor::GetColor("#990000"));
                    Colors.push_back(TColor::GetColor("#009900"));
                    Colors.push_back(TColor::GetColor("#000099"));
                } else if (Num==3) {
                    styles[0] = kSolid  ;
                    styles[1] = kSolid  ;
                    styles[2] = kSolid  ;
                    Colors.push_back(TColor::GetColor("#990000"));
                    Colors.push_back(TColor::GetColor("#009900"));
                    Colors.push_back(TColor::GetColor("#000099"));
                } else if ((Num==4)&&compare) {
                    styles[0] = kSolid  ;
                    styles[1] = kSolid  ;
                    styles[2] = kDashed ;
                    styles[3] = kDashed ;
                    Colors.push_back(TColor::GetColor("#990000"));
                    Colors.push_back(TColor::GetColor("#000099"));
                    Colors.push_back(TColor::GetColor("#990000"));
                    Colors.push_back(TColor::GetColor("#000099"));
                } else if (Num==2) {
                    Colors.push_back(TColor::GetColor("#990000"));
                    Colors.push_back(TColor::GetColor("#000099"));
                } else {
                    Colors.push_back(TColor::GetColor("#990000"));
                    Colors.push_back(TColor::GetColor("#009900"));
                    Colors.push_back(TColor::GetColor("#000099"));
                    Colors.push_back(TColor::GetColor("#999900"));
                    Colors.push_back(TColor::GetColor("#990099"));
                    Colors.push_back(TColor::GetColor("#009999"));
                    Colors.push_back(TColor::GetColor("#000000"));
                }
            }
            double YMax = 0 ;
            for(size_t i=0;i<Num;i++){
                NormalizeHist(Hists[i]);
                int    binmax = Hists[i]->GetMaximumBin ()            ;
                double TmpMax = Hists[i]->GetBinContent (binmax)      ;
                YMax          = CPPFileIO::mymax        (YMax,TmpMax) ;
            }
            for(size_t i=0;i<Num;i++){
				if(fixedy){YMax=0.1;}
                Hists[i]->SetMaximum   ( YMax        ) ;
                Hists[i]->SetLineColor ( Colors[i]   ) ;
                Hists[i]->SetLineStyle ( styles[i]   ) ;
                Hists[i]->Draw         ( "hist same" ) ;
            }
            mkdir ( "./OUTS/"        , 0755 ) ;
            mkdir ( "./OUTS/GRAPHS/" , 0755 ) ;
            std::string outfilename ("./OUTS/GRAPHS/") ;
            std::string outname  = outfilename + histname + ".pdf" ;
            std::string outnameC = outfilename + histname + ".C"   ;
            C.SaveAs ( & ( outname[0]  ) ) ;
            C.SaveAs ( & ( outnameC[0] ) ) ;
            DeleteAll () ;
        }
        inline void AllocateAll (size_t nbins, double min, double max) {
            for(size_t ii=0;ii<Num;ii++){
                char tmp[1024];
                sprintf(tmp,"%s_%ld",&(histname[0]),ii);
                histNnames[ii] = std::string(tmp);
                Hists[ii] = new TH1F ( & ( histNnames[ii][0] ) , & ( histname [0] ) , nbins , min , max ) ;
            }
        }
        inline void SetLogScale (bool _logscale) {logscale=_logscale;}
        MyHistN  (std::string _histname, size_t nbins, double min, double max) : histname(_histname)
        { AllocateAll (nbins,min,max) ; logscale=false; fixedy=false; }
        ~MyHistN () { Write () ; }
        bool fixedy ;
    };
    //
    class Plot2D {
    public:
        //
        using FileReader =
            CPPFileIO::FullFileReader
                <imagetype>
        ; //
        //
        std::string histname ;
        //
        TH2F Hist ;
        //
        inline void
        operator () (
            std::string filename
        ) {
            FileReader reader (filename) ;

            for (size_t i=0;i<reader();i++) {
                TYPE_DATA sum = 0 ;
                for (size_t y=0;y<ImageResolution;y++)
                for (size_t x=0;x<ImageResolution;x++) {
                    Hist.Fill (
                        x , y ,
                        reader(i)
                            .Image[y][x]
                    ) ; //
                    sum +=
                        reader(i)
                            .Image[y][x]
                    ; //
                }
                //printf("DEBUG: %e\n",sum);
            }
        }
        //
        Plot2D (
            std::string filename
        ) :
        Hist (
            &(filename[0]) ,
            &(filename[0]) ,
            ImageResolution , 0 , ImageResolution ,
            ImageResolution , 0 , ImageResolution
        ) { histname = filename ; }
        //
        ~Plot2D(){
            mkdir("./OUTS/",0755);
            mkdir("./OUTS/GRAPHS/",0755);
            std::string outfilename("./OUTS/GRAPHS/");
            outfilename =
                outfilename +
                histname +
                ".pdf"
            ; //
            TCanvas C;
            Hist.Draw("colz");
            C.SaveAs(
                &(outfilename[0])
            );
        }
        //
    } ;
    //
    class Plot2D_1 {
    public:
        //
        using FileReader =
            CPPFileIO::FullFileReader
                <imagetype>
        ; //
        //
        std::string histname ;
        //
        TH2F Hist ;
        //
        inline void
        operator () (
            std::string filename
        ) {
            FileReader reader (filename) ;

            for (size_t i=0;(i<reader())&&(i<1);i++) {
                TYPE_DATA sum = 0 ;
                for (size_t y=0;y<ImageResolution;y++)
                for (size_t x=0;x<ImageResolution;x++) {
                    Hist.Fill (
                        x , y ,
                        reader(i)
                            .Image[y][x]
                    ) ; //
                    sum +=
                        reader(i)
                            .Image[y][x]
                    ; //
                }
                //printf("DEBUG: %e\n",sum);
            }
        }
        //
        Plot2D_1 (
            std::string filename
        ) :
        Hist (
            &(filename[0]) ,
            &(filename[0]) ,
            ImageResolution , 0 , ImageResolution ,
            ImageResolution , 0 , ImageResolution
        ) { histname = filename ; }
        //
        ~Plot2D_1(){
            mkdir("./OUTS/",0755);
            mkdir("./OUTS/GRAPHS/",0755);
            std::string outfilename("./OUTS/GRAPHS/");
            outfilename =
                outfilename +
                histname +
                ".pdf"
            ; //
            TCanvas C;
            Hist.Draw("colz");
            C.SaveAs(
                &(outfilename[0])
            );
        }
        //
    } ;
    //
    class ROC_ELEMENT {
    public:
        //
        TYPE_DATA value ;
        bool signal ;
        //
        ROC_ELEMENT (
            TYPE_DATA _value = 0.0 ,
            bool _signal = false
        ) {
            value  = _value  ;
            signal = _signal ;
        }
        //
        ~ROC_ELEMENT(){}
        //
        inline bool
        operator > (
            ROC_ELEMENT const &
                other
        ) {
            return
                value > other.value
            ; //
        }
        //
        inline bool
        operator < (
            ROC_ELEMENT const &
                other
        ) {
            return
                value < other.value
            ; //
        }
        //
    } ;
    //
    using ROC_ELEMENTs =
        std::vector
            <ROC_ELEMENT>
    ; //
    //
    inline void
    WriteROC (
        std::vector <ROC_ELEMENT> &
            invals ,
        std::string filename
    ) {
        std::sort (
            invals.begin () ,
            invals.end   ()
        ) ; //
        size_t TotalSig = 0 ;
        size_t TotalBkg = 0 ;
        for(size_t i=0;i<invals.size();i++){
            if(invals[i].signal){TotalSig++;}
            else{TotalBkg++;}
        }
        size_t CurrentSig = 0 ;
        size_t CurrentBkg = 0 ;
        FILE *f =
            fopen (
                &(filename[0]),
                "w"
            )
        ; /* Write to the file: */ {
            for(size_t i=0;i<invals.size();i++){
                if(invals[i].signal){CurrentSig++;}
                else{CurrentBkg++;}
                TYPE_DATA EpsS =
                    ((TYPE_DATA)CurrentSig) /
                    ((TYPE_DATA)TotalSig)
                ; //
                TYPE_DATA EpsB =
                    ((TYPE_DATA)CurrentBkg) /
                    ((TYPE_DATA)TotalBkg)
                ; //
                fprintf(f,"%e %e\n",EpsS,1.0/EpsB);
            }
        } fclose(f) ;
    }
    //
    inline void
    WriteHTTROC (
        std::vector <ROC_ELEMENT> &
            invals ,
        std::string filename
    ) {

        size_t TotalSig = 0 ;
        size_t TotalBkg = 0 ;
        size_t CurrSig  = 0 ;
        size_t CurrBkg  = 0 ;

        for (size_t i=0;i<invals.size();i++) {
            if (invals[i].signal) {
                TotalSig++;
                if (invals[i].value>0.5) {
                    CurrSig++;
                }
            } else {
                TotalBkg++;
                if (invals[i].value>0.5) {
                    CurrBkg++;
                }
            }
        }
        printf("DEBUG: %ld %ld %ld %ld",TotalSig,CurrSig,TotalBkg,CurrBkg);

        TYPE_DATA EpsSig =
            (TYPE_DATA(CurrSig)) /
            (TYPE_DATA(TotalSig))
        ; //
        TYPE_DATA EpsBkg =
            (TYPE_DATA(CurrBkg)) /
            (TYPE_DATA(TotalBkg))
        ; //

        FILE * f = fopen (&(filename[0]),"w") ;
        fprintf(f,"%e %e\n",EpsSig,1.0/EpsBkg);
        fclose(f);

    }
    //
}
////////////////////////////////////////////////////////////////
namespace STEP1_GENERATION {

    using namespace MISC ;

    inline void
    Generate_QCD (
        size_t      index                ,
        std::string MidName              ,
        size_t      random_seed = 123    ,
        size_t      N_Events    = 100000
    ) {
        CPPFileIO::SetCPUAffinity(index);
        std::string prefixname ;
        /* Make the directories: */ {
            std::string dirname = "./OUTS/" ;
            mkdir ( &(dirname[0]) , 0755 ) ;
            dirname = "./OUTS/QCD/" ;
            mkdir ( &(dirname[0]) , 0755 ) ;
            dirname = "./OUTS/QCD/" + MidName + "/" ;
            mkdir ( &(dirname[0]) , 0755 ) ;
            char tmp = '0' ;
            tmp = tmp + ( (char) index ) ;
            dirname = dirname + tmp + "/" ;
            mkdir ( &(dirname[0]) , 0755 ) ;
            prefixname = dirname ;
        }
        size_t count = 0 ;
        EventWriter
            writer (
                prefixname
            )
        ; //
        MyPythia
            pythia
        ; /* Configure pythia: */ {
            pythia.readString ( "Beams:eCM = 13000"         ) ;
            pythia.readString ( "PhaseSpace:pTHatMin = 750" ) ;
            pythia.readString ( "PhaseSpace:pTHatMax = 950" ) ;
            pythia.readString ( "PartonLevel:MPI = off"     ) ;
            pythia.readString ( "Random:setSeed = on"       ) ;
            /* Set the random number Seed */ {
                char tmp[256] ;
                sprintf (
                    tmp                 ,
                    "Random:seed = %ld" ,
                    index + random_seed
                ) ; //
                pythia.readString
                    (tmp)
                ; //
            }
            pythia.readString ( "HardQCD:all = on" ) ;
            pythia.init () ;
        }
        while (
            count<N_Events
        ) if (
            pythia.next()
        ) {
            vector4s hadrons ;
            /* Prepare the list of hadrons: */ {
                for (
                    size_t i=0;
                    i<pythia.event.size();
                    i++
                ) if (
                    pythia.event[i]
                        .isFinal()
                ) {
                    int pid =
                        pythia
                        .event[i]
                        .id()
                    ; //
                    if (pid<0) {pid=-pid;}
                    if (
                        ( pid != 12 ) &&
                        ( pid != 14 ) &&
                        ( pid != 16 )
                    ) {
                        vector4 tmp (
                            pythia.event[i].px() ,
                            pythia.event[i].py() ,
                            pythia.event[i].pz() ,
                            pythia.event[i].e()
                        ) ;
                        auto tmpeta =
                            tmp.rapidity()
                        ; //
                        if (
                            ( -2.5     < tmpeta ) &&
                            ( tmpeta   < 2.5    ) &&
                            ( tmp.pt() > 0.5    )
                        ) {
                            hadrons.push_back
                                (tmp)
                            ; //
                        }
                    }
                }
            }
            fastjet::JetAlgorithm algo =
                fastjet::antikt_algorithm
            ; //
            fastjet::JetDefinition
                jet_def (
                    algo,
                    1.0
                )
            ; //
            fastjet::ClusterSequence
                clust_seq (
                    hadrons,
                    jet_def
                )
            ; //
            vector4s jets =
                clust_seq
                .inclusive_jets
                    (800.0)
            ; //
            if (
                (jets.size()>0) &&
                (jets[0].constituents().size()>2)
            ) {
                auto tmppt =
                    jets[0].pt ()
                ; //
                auto tmpet =
                    jets[0].rapidity ()
                ; //
                if (
                    (
                        ( 800 < tmppt ) &&
                        ( tmppt < 900 )
                    ) && (
                        ( -2.5 < tmpet ) &&
                        ( tmpet < 2.5 )
                    )
                ) {
                    writer(jets[0]);
                    count++ ;
                }
            }
        }
    }

    inline void
    Generate_Train () {
        CPPFileIO::ForkMe forker ;
        for(size_t i=0;i<8;i++)
        if(forker.InKid()){
            Generate_QCD (
                i       ,
                "TRAIN" ,
                123     ,
                300000
            ) ; //
        }
    }

	inline void
	Generate_Test () {
		CPPFileIO::ForkMe forker ;
		for(size_t i=0;i<8;i++)
		if(forker.InKid()){
			Generate_QCD (
				i      ,
				"TEST" ,
				456    ,
				50000
			) ; //
		}
	}

    inline void
    PlotImages () {

        /* TRAIN */ if(true) {

            /* NORMAL */ {
                Plot2D
                    tmp("QCD_TRAIN_IMAGES")
                ; /* Record the file names: */ {
                    tmp("./OUTS/QCD/TRAIN/0/image");
                    tmp("./OUTS/QCD/TRAIN/1/image");
                    tmp("./OUTS/QCD/TRAIN/2/image");
                    tmp("./OUTS/QCD/TRAIN/3/image");
                    tmp("./OUTS/QCD/TRAIN/4/image");
                    tmp("./OUTS/QCD/TRAIN/5/image");
                    tmp("./OUTS/QCD/TRAIN/6/image");
                    tmp("./OUTS/QCD/TRAIN/7/image");
                }
            }

            /* UNBOOSTED */ {
                Plot2D
                    tmp("QCD_TRAIN_IMAGES_UNBOOSTED")
                ; /* Record the file names: */ {
                    tmp("./OUTS/QCD/TRAIN/0/UnBoostedimage");
                    tmp("./OUTS/QCD/TRAIN/1/UnBoostedimage");
                    tmp("./OUTS/QCD/TRAIN/2/UnBoostedimage");
                    tmp("./OUTS/QCD/TRAIN/3/UnBoostedimage");
                    tmp("./OUTS/QCD/TRAIN/4/UnBoostedimage");
                    tmp("./OUTS/QCD/TRAIN/5/UnBoostedimage");
                    tmp("./OUTS/QCD/TRAIN/6/UnBoostedimage");
                    tmp("./OUTS/QCD/TRAIN/7/UnBoostedimage");
                }
            }

            /* NOGRAMSCHMIDT */ {
                Plot2D
                    tmp("QCD_TRAIN_IMAGES_NOGRAMSCHMIDT")
                ; /* Record the file names: */ {
                    tmp("./OUTS/QCD/TRAIN/0/NoGramSchmidtImageType");
                    tmp("./OUTS/QCD/TRAIN/1/NoGramSchmidtImageType");
                    tmp("./OUTS/QCD/TRAIN/2/NoGramSchmidtImageType");
                    tmp("./OUTS/QCD/TRAIN/3/NoGramSchmidtImageType");
                    tmp("./OUTS/QCD/TRAIN/4/NoGramSchmidtImageType");
                    tmp("./OUTS/QCD/TRAIN/5/NoGramSchmidtImageType");
                    tmp("./OUTS/QCD/TRAIN/6/NoGramSchmidtImageType");
                    tmp("./OUTS/QCD/TRAIN/7/NoGramSchmidtImageType");
                }
            }

        }

        /* TEST */ if(true) {

            /* NORMAL */ {
                Plot2D
                    tmp("QCD_TEST_IMAGES")
                ; /* Record the file names: */ {
                    tmp("./OUTS/QCD/TEST/0/image");
                    tmp("./OUTS/QCD/TEST/1/image");
                    tmp("./OUTS/QCD/TEST/2/image");
                    tmp("./OUTS/QCD/TEST/3/image");
                    tmp("./OUTS/QCD/TEST/4/image");
                    tmp("./OUTS/QCD/TEST/5/image");
                    tmp("./OUTS/QCD/TEST/6/image");
                    tmp("./OUTS/QCD/TEST/7/image");
                }
            }

            /* UNBOOSTED */ {
                Plot2D
                    tmp("QCD_TEST_IMAGES_UNBOOSTED")
                ; /* Record the file names: */ {
                    tmp("./OUTS/QCD/TEST/0/UnBoostedimage");
                    tmp("./OUTS/QCD/TEST/1/UnBoostedimage");
                    tmp("./OUTS/QCD/TEST/2/UnBoostedimage");
                    tmp("./OUTS/QCD/TEST/3/UnBoostedimage");
                    tmp("./OUTS/QCD/TEST/4/UnBoostedimage");
                    tmp("./OUTS/QCD/TEST/5/UnBoostedimage");
                    tmp("./OUTS/QCD/TEST/6/UnBoostedimage");
                    tmp("./OUTS/QCD/TEST/7/UnBoostedimage");
                }
            }

            /* NOGRAMSCHMIDT */ {
                Plot2D
                    tmp("QCD_TEST_IMAGES_NOGRAMSCHMIDT")
                ; /* Record the file names: */ {
                    tmp("./OUTS/QCD/TEST/0/NoGramSchmidtImageType");
                    tmp("./OUTS/QCD/TEST/1/NoGramSchmidtImageType");
                    tmp("./OUTS/QCD/TEST/2/NoGramSchmidtImageType");
                    tmp("./OUTS/QCD/TEST/3/NoGramSchmidtImageType");
                    tmp("./OUTS/QCD/TEST/4/NoGramSchmidtImageType");
                    tmp("./OUTS/QCD/TEST/5/NoGramSchmidtImageType");
                    tmp("./OUTS/QCD/TEST/6/NoGramSchmidtImageType");
                    tmp("./OUTS/QCD/TEST/7/NoGramSchmidtImageType");
                }
            }

			/* NOGRAMSCHMIDT_BAD */ {
				Plot2D
					tmp("QCD_TEST_IMAGES_NOGRAMSCHMIDT_BAD")
				; /* Record the file names: */ {
					tmp("./OUTS/QCD/TEST/0/NoGramSchmidtBADImageType");
					tmp("./OUTS/QCD/TEST/1/NoGramSchmidtBADImageType");
					tmp("./OUTS/QCD/TEST/2/NoGramSchmidtBADImageType");
					tmp("./OUTS/QCD/TEST/3/NoGramSchmidtBADImageType");
					tmp("./OUTS/QCD/TEST/4/NoGramSchmidtBADImageType");
					tmp("./OUTS/QCD/TEST/5/NoGramSchmidtBADImageType");
					tmp("./OUTS/QCD/TEST/6/NoGramSchmidtBADImageType");
					tmp("./OUTS/QCD/TEST/7/NoGramSchmidtBADImageType");
				}
			}

        }

    }

    inline void
    PlotImages_1 () {

        /* TRAIN */ if(true) {

            /* NORMAL */ {
                Plot2D_1
                    tmp("QCD_TRAIN_IMAGES_1")
                ; /* Record the file names: */ {
                    tmp("./OUTS/QCD/TRAIN/0/image");
                }
            }

            /* UNBOOSTED */ {
                Plot2D_1
                    tmp("QCD_TRAIN_IMAGES_UNBOOSTED_1")
                ; /* Record the file names: */ {
                    tmp("./OUTS/QCD/TRAIN/0/UnBoostedimage");
                }
            }

            /* NOGRAMSCHMIDT */ {
                Plot2D_1
                    tmp("QCD_TRAIN_IMAGES_NOGRAMSCHMIDT_1")
                ; /* Record the file names: */ {
                    tmp("./OUTS/QCD/TRAIN/0/NoGramSchmidtImageType");
                }
            }

        }

    }

    inline void
    PlotNsub () {
        //
        /* TRAIN */ if(true) {
            MyHistN <5,false>
                nsubcompare (
                    "TAU_QCD_TRAIN",100,
                    -0.01,100.01
                )
            ; //
            for(
                size_t index=0;
                index<8;
                index++
            ) {
                char tmp[256] ;
                sprintf (
                    tmp,
                    "./OUTS/QCD/TRAIN/%ld/nsub",
                    index
                ) ; //
                CPPFileIO::FullFileReader
                    <TYPE_NSub>
                        Reader (tmp)
                ; //
                for (
                    size_t i=0;
                    i<Reader();
                    i++
                ) {
                    //
                    nsubcompare.Fill<0>
                        (Reader(i)[0])
                    ; //
                    nsubcompare.Fill<1>
                        (Reader(i)[1])
                    ; //
                    nsubcompare.Fill<2>
                        (Reader(i)[2])
                    ; //
                    nsubcompare.Fill<3>
                        (Reader(i)[3])
                    ; //
                    nsubcompare.Fill<4>
                        (Reader(i)[4])
                    ; //
                    //
                }
            }
        }
        //
        /* TEST */ if(true) {
            MyHistN <5,false>
                nsubcompare (
                    "TAU_QCD_TEST",100,
                    -0.01,100.01
                )
            ; //
            for(
                size_t index=0;
                index<8;
                index++
            ) {
                char tmp[256] ;
                sprintf (
                    tmp,
                    "./OUTS/QCD/TEST/%ld/nsub",
                    index
                ) ; //
                CPPFileIO::FullFileReader
                    <TYPE_NSub>
                        Reader (tmp)
                ; //
                for (
                    size_t i=0;
                    i<Reader();
                    i++
                ) {
                    //
                    nsubcompare.Fill<0>
                        (Reader(i)[0])
                    ; //
                    nsubcompare.Fill<1>
                        (Reader(i)[1])
                    ; //
                    nsubcompare.Fill<2>
                        (Reader(i)[2])
                    ; //
                    nsubcompare.Fill<3>
                        (Reader(i)[3])
                    ; //
                    nsubcompare.Fill<4>
                        (Reader(i)[4])
                    ; //
                    //
                }
            }
        }
        //
    }

}
////////////////////////////////////////////////////////////////
namespace STEP2_GENERATEWBS {
    //
    using namespace MISC ;
    //
    inline void
    Generate_WBS (
        size_t      index                ,
        std::string MidName              ,
        size_t      random_seed = 123    ,
        size_t      N_Events    = 100000
    ) {
        CPPFileIO::SetCPUAffinity(index);
        std::string prefixname ;
        /* Make the directories: */ {
            std::string dirname = "./OUTS/" ;
            mkdir ( &(dirname[0]) , 0755 ) ;
            dirname = "./OUTS/WBS/" ;
            mkdir ( &(dirname[0]) , 0755 ) ;
            dirname = "./OUTS/WBS/" + MidName + "/" ;
            mkdir ( &(dirname[0]) , 0755 ) ;
            char tmp = '0' ;
            tmp = tmp + ( (char) index ) ;
            dirname = dirname + tmp + "/" ;
            mkdir ( &(dirname[0]) , 0755 ) ;
            prefixname = dirname ;
        }
        size_t count = 0 ;
        EventWriter writer (prefixname) ;
        MyPythia pythia ; /* Configure pythia: */ {
            pythia.readString ( "Beams:eCM = 13000"         ) ;
            pythia.readString ( "PhaseSpace:pTHatMin = 750" ) ;
            pythia.readString ( "PhaseSpace:pTHatMax = 950" ) ;
            pythia.readString ( "PartonLevel:MPI = off"     ) ;
            pythia.readString ( "Random:setSeed = on"       ) ;
            /* Set the random number Seed */ {
                char tmp[256] ;
                sprintf (
                    tmp                 ,
                    "Random:seed = %ld" ,
                    index + random_seed
                ) ; //
                pythia.readString
                    (tmp)
                ; //
            }
            pythia.readString ( "WeakDoubleBoson:ffbar2ZW = on" ) ;
            pythia.readString ( "23:onMode = off"               ) ;
            pythia.readString ( "23:onIfAny = 12"               ) ;
            pythia.readString ( "24:onMode = off"               ) ;
            pythia.readString ( "24:onIfAny = 1 2 3 4"          ) ;
            pythia.init () ;
        }
        while (count<N_Events)
        if (pythia.next()) {
            vector4s hadrons ;
            /* Prepare the list of hadrons: */ {
                for (
                    size_t i=0;
                    i<pythia.event.size();
                    i++
                ) if (
                    pythia.event[i]
                        .isFinal()
                ) {
                    int pid =
                        pythia
                        .event[i]
                        .id()
                    ;
                    if (pid<0) {pid=-pid;}
                    if (
                        ( pid != 12 ) &&
                        ( pid != 14 ) &&
                        ( pid != 16 )
                    ) {
                        vector4 tmp (
                            pythia.event[i].px () ,
                            pythia.event[i].py () ,
                            pythia.event[i].pz () ,
                            pythia.event[i].e  ()
                        ) ;
                        auto tmpeta =
                            tmp.rapidity()
                        ;
                        if (
                            ( -2.5     < tmpeta ) &&
                            ( tmpeta   < 2.5    ) &&
                            ( tmp.pt() > 0.5    )
                        ) {
                            hadrons.push_back
                                (tmp)
                            ; //
                        }
                    }
                }
            }
            //
            vector4 WBS , JT[2] ;
            /* Get the GEN Vectors: */ {
                long wid =
                    pythia.Recurse (
                        pythia.FindParticleMod
                            (24)
                    ) ;
                WBS = pythia.GetVector(wid);
                long j1id[2] = {
                    pythia.event[wid]
                        .daughter1 () ,
                    pythia.event[wid]
                        .daughter1 ()
                } ;
                JT[0] =
                    pythia
                    .GetVector
                        (j1id[0])
                ;
                JT[1] =
                    pythia
                    .GetVector
                        (j1id[1])
                ;
            }
            //
            fastjet::JetAlgorithm algo =
                fastjet::antikt_algorithm
            ; //
            fastjet::JetDefinition
                jet_def (
                    algo,
                    1.0
                )
            ;
            fastjet::ClusterSequence
                clust_seq (
                    hadrons,
                    jet_def
                )
            ;
            vector4s jets =
                clust_seq
                .inclusive_jets
                        (800.0)
            ;
            //
            if (jets.size()>0) {
                //
                bool genmatched = (
                    ( jets[0].delta_R (   WBS ) < 0.6 ) &&
                    ( jets[0].delta_R ( JT[0] ) < 0.6 ) &&
                    ( jets[0].delta_R ( JT[1] ) < 0.6 )
                ) ;
                //
                if (
                    (genmatched) &&
                    (jets[0].constituents().size()>2)
                ) {
                    auto tmppt =
                        jets[0]
                        .pt()
                    ; //
                    auto tmpet =
                        jets[0]
                        .rapidity()
                    ; //
                    if (
                        (
                            ( 800 < tmppt ) &&
                            ( tmppt < 900 )
                        ) && (
                            ( -2.5 < tmpet ) &&
                            ( tmpet < 2.5 )
                        )
                    ) {
                        writer(jets[0]);
                        count++ ;
                    }
                }
            }
        }
    }
    //
    inline void
    Generate_Train_WBS () {
        CPPFileIO::ForkMe forker ;
        for (size_t i=0;i<8;i++)
        if (forker.InKid()) {
            Generate_WBS (
                i       ,
                "TRAIN" ,
                123     ,
                300000
            ) ; //
        }
    }
    //
    inline void
    Generate_Test_WBS () {
        CPPFileIO::ForkMe forker ;
        for (size_t i=0;i<8;i++)
        if (forker.InKid()) {
            Generate_WBS (
                i      ,
                "TEST" ,
                456    ,
                50000
            ) ; //
        }
    }
    //
    inline void
    PlotImages () {

        /* TRAIN */ if(true) {

            /* NORMAL */ if(true) {
                Plot2D tmp("WBS_TRAIN_IMAGES");
                tmp("./OUTS/WBS/TRAIN/0/image");
                tmp("./OUTS/WBS/TRAIN/1/image");
                tmp("./OUTS/WBS/TRAIN/2/image");
                tmp("./OUTS/WBS/TRAIN/3/image");
                tmp("./OUTS/WBS/TRAIN/4/image");
                tmp("./OUTS/WBS/TRAIN/5/image");
                tmp("./OUTS/WBS/TRAIN/6/image");
                tmp("./OUTS/WBS/TRAIN/7/image");
            }

            /* UNBOOSTED */ if(true) {
                Plot2D tmp("WBS_TRAIN_IMAGES_UNBOOSTED");
                tmp("./OUTS/WBS/TRAIN/0/UnBoostedimage");
                tmp("./OUTS/WBS/TRAIN/1/UnBoostedimage");
                tmp("./OUTS/WBS/TRAIN/2/UnBoostedimage");
                tmp("./OUTS/WBS/TRAIN/3/UnBoostedimage");
                tmp("./OUTS/WBS/TRAIN/4/UnBoostedimage");
                tmp("./OUTS/WBS/TRAIN/5/UnBoostedimage");
                tmp("./OUTS/WBS/TRAIN/6/UnBoostedimage");
                tmp("./OUTS/WBS/TRAIN/7/UnBoostedimage");
            }

            /* NOGRAMSCHMIDT */ if(true) {
                Plot2D tmp("WBS_TRAIN_IMAGES_NOGRAMSCHMIDT");
                tmp("./OUTS/WBS/TRAIN/0/NoGramSchmidtImageType");
                tmp("./OUTS/WBS/TRAIN/1/NoGramSchmidtImageType");
                tmp("./OUTS/WBS/TRAIN/2/NoGramSchmidtImageType");
                tmp("./OUTS/WBS/TRAIN/3/NoGramSchmidtImageType");
                tmp("./OUTS/WBS/TRAIN/4/NoGramSchmidtImageType");
                tmp("./OUTS/WBS/TRAIN/5/NoGramSchmidtImageType");
                tmp("./OUTS/WBS/TRAIN/6/NoGramSchmidtImageType");
                tmp("./OUTS/WBS/TRAIN/7/NoGramSchmidtImageType");
            }

        }

        /* TEST */ if(true) {

            /* NORMAL */ if(true) {
                Plot2D tmp("WBS_TEST_IMAGES");
                tmp("./OUTS/WBS/TEST/0/image");
                tmp("./OUTS/WBS/TEST/1/image");
                tmp("./OUTS/WBS/TEST/2/image");
                tmp("./OUTS/WBS/TEST/3/image");
                tmp("./OUTS/WBS/TEST/4/image");
                tmp("./OUTS/WBS/TEST/5/image");
                tmp("./OUTS/WBS/TEST/6/image");
                tmp("./OUTS/WBS/TEST/7/image");
            }

            /* UNBOOSTED */ if(true) {
                Plot2D tmp("WBS_TEST_IMAGES_UNBOOSTED");
                tmp("./OUTS/WBS/TEST/0/UnBoostedimage");
                tmp("./OUTS/WBS/TEST/1/UnBoostedimage");
                tmp("./OUTS/WBS/TEST/2/UnBoostedimage");
                tmp("./OUTS/WBS/TEST/3/UnBoostedimage");
                tmp("./OUTS/WBS/TEST/4/UnBoostedimage");
                tmp("./OUTS/WBS/TEST/5/UnBoostedimage");
                tmp("./OUTS/WBS/TEST/6/UnBoostedimage");
                tmp("./OUTS/WBS/TEST/7/UnBoostedimage");
            }

            /* NOGRAMSCHMIDT */ if(true) {
                Plot2D tmp("WBS_TEST_IMAGES_NOGRAMSCHMIDT");
                tmp("./OUTS/WBS/TEST/0/NoGramSchmidtImageType");
                tmp("./OUTS/WBS/TEST/1/NoGramSchmidtImageType");
                tmp("./OUTS/WBS/TEST/2/NoGramSchmidtImageType");
                tmp("./OUTS/WBS/TEST/3/NoGramSchmidtImageType");
                tmp("./OUTS/WBS/TEST/4/NoGramSchmidtImageType");
                tmp("./OUTS/WBS/TEST/5/NoGramSchmidtImageType");
                tmp("./OUTS/WBS/TEST/6/NoGramSchmidtImageType");
                tmp("./OUTS/WBS/TEST/7/NoGramSchmidtImageType");
            }

        }

    }
    //
    inline void
    PlotNsub () {
        //
        /* TRAIN */ if(true) {
            MyHistN <5,false>
                nsubcompare (
                    "TAU_WBS_TRAIN",100,
                    -0.01,100.01
                )
            ; //
            for(
                size_t index=0;
                index<8;
                index++
            ) {
                char tmp[256] ;
                sprintf (
                    tmp,
                    "./OUTS/WBS/TRAIN/%ld/nsub",
                    index
                ) ; //
                CPPFileIO::FullFileReader
                    <TYPE_NSub>
                        Reader (tmp)
                ; //
                for (
                    size_t i=0;
                    i<Reader();
                    i++
                ) {
                    //
                    nsubcompare.Fill<0>
                        (Reader(i)[0])
                    ; //
                    nsubcompare.Fill<1>
                        (Reader(i)[1])
                    ; //
                    nsubcompare.Fill<2>
                        (Reader(i)[2])
                    ; //
                    nsubcompare.Fill<3>
                        (Reader(i)[3])
                    ; //
                    nsubcompare.Fill<4>
                        (Reader(i)[4])
                    ; //
                    //
                }
            }
        }
        //
        /* TEST */ if(true) {
            MyHistN <5,false>
                nsubcompare (
                    "TAU_WBS_TEST",100,
                    -0.01,100.01
                )
            ; //
            for(
                size_t index=0;
                index<8;
                index++
            ) {
                char tmp[256] ;
                sprintf (
                    tmp,
                    "./OUTS/WBS/TEST/%ld/nsub",
                    index
                ) ; //
                CPPFileIO::FullFileReader
                    <TYPE_NSub>
                        Reader (tmp)
                ; //
                for (
                    size_t i=0;
                    i<Reader();
                    i++
                ) {
                    //
                    nsubcompare.Fill<0>
                        (Reader(i)[0])
                    ; //
                    nsubcompare.Fill<1>
                        (Reader(i)[1])
                    ; //
                    nsubcompare.Fill<2>
                        (Reader(i)[2])
                    ; //
                    nsubcompare.Fill<3>
                        (Reader(i)[3])
                    ; //
                    nsubcompare.Fill<4>
                        (Reader(i)[4])
                    ; //
                    //
                }
            }
        }
        //
    }
    //
}
////////////////////////////////////////////////////////////////
namespace STEP3_GENERATETOP {
    //
    using namespace MISC ;
    //
    inline void
    Generate_TOP (
        size_t      index                ,
        std::string MidName              ,
        size_t      random_seed = 123    ,
        size_t      N_Events    = 100000
    ) {
        CPPFileIO::SetCPUAffinity(index);
        std::string prefixname ;
        /* Make the directories: */ {
            std::string dirname = "./OUTS/" ;
            mkdir ( &(dirname[0]) , 0755 ) ;
            dirname = "./OUTS/TOP/" ;
            mkdir ( &(dirname[0]) , 0755 ) ;
            dirname = "./OUTS/TOP/" + MidName + "/" ;
            mkdir ( &(dirname[0]) , 0755 ) ;
            char tmp = '0' ;
            tmp = tmp + ( (char) index ) ;
            dirname = dirname + tmp + "/" ;
            mkdir ( &(dirname[0]) , 0755 ) ;
            prefixname = dirname ;
        }
        size_t count = 0 ;
        EventWriter writer (prefixname) ;
        MyPythia pythia ;
        /* Configure pythia: */ {
            pythia.readString ( "Beams:eCM = 13000"         ) ;
            pythia.readString ( "PhaseSpace:pTHatMin = 750" ) ;
            pythia.readString ( "PhaseSpace:pTHatMax = 950" ) ;
            pythia.readString ( "PartonLevel:MPI = off"     ) ;
            pythia.readString ( "Random:setSeed = on"       ) ;
            /* Set the random number Seed */ {
                char tmp[256] ;
                sprintf (
                    tmp                 ,
                    "Random:seed = %ld" ,
                    index + random_seed
                ) ; //
                pythia.readString
                    (tmp)
                ; //
            }
            pythia.readString ( "Top:gg2ttbar = on"    ) ;
            pythia.readString ( "24:onMode = off"      ) ;
            pythia.readString ( "24:onIfAny = 1 2 3 4" ) ;
            pythia.init () ;
        }
        while (count<N_Events)
        if (pythia.next()) {
            vector4s hadrons ;
            /* Prepare the list of hadrons: */ {
                for (
                    size_t i=0;
                    i<pythia.event.size();
                    i++
                ) if (
                    pythia.event[i]
                        .isFinal()
                ) {
                    int pid =
                        pythia
                        .event[i]
                        .id()
                    ;
                    if (pid<0) {pid=-pid;}
                    if (
                        ( pid != 12 ) &&
                        ( pid != 14 ) &&
                        ( pid != 16 )
                    ) {
                        vector4 tmp (
                            pythia.event[i].px () ,
                            pythia.event[i].py () ,
                            pythia.event[i].pz () ,
                            pythia.event[i].e  ()
                        ) ;
                        auto tmpeta =
                            tmp.rapidity()
                        ;
                        if (
                            ( -2.5     < tmpeta ) &&
                            ( tmpeta   < 2.5    ) &&
                            ( tmp.pt() > 0.5    )
                        ) {
                            hadrons.push_back
                                (tmp)
                            ; //
                        }
                    }
                }
            }
            //
            vector4
                TOP[2] , WBS[2] , BOT[2] ,
                JT1[2] , JT2[2]
            ; /* Get the GEN Vectors: */ {
                long tid[2] = {
                    pythia.Recurse (
                        pythia.FindParticle
                            (+6)
                    ) ,
                    pythia.Recurse (
                        pythia.FindParticle
                            (-6)
                    )
                } ;
                TOP[0] =
                    pythia
                    .GetVector
                        (tid[0])
                ; //
                TOP[1] =
                    pythia
                    .GetVector
                        (tid[1])
                ; //
                long wid[2] = {
                    pythia.Recurse (
                        pythia.FindDaughter (
                            (long) /* ParentID = */ tid[0] ,
                            (long) /* DPID     = */ +24
                        )
                    ) ,
                    pythia.Recurse (
                        pythia.FindDaughter (
                            (long) /* ParentID = */ tid[1] ,
                            (long) /* DPID     = */ -24
                        )
                    )
                } ;
                WBS[0] =
                    pythia
                    .GetVector
                        (wid[0])
                ; //
                WBS[1] =
                    pythia
                    .GetVector
                        (wid[1])
                ; //
                long j1id[2] = {
                    pythia.event[wid[0]]
                        .daughter1 () ,
                    pythia.event[wid[1]]
                        .daughter1 ()
                } ;
                JT1[0] =
                    pythia
                    .GetVector
                        (j1id[0])
                ; //
                JT1[1] =
                    pythia
                    .GetVector
                        (j1id[1])
                ; //
                long j2id[2] = {
                    pythia.event[wid[0]]
                        .daughter2 () ,
                    pythia.event[wid[1]]
                        .daughter2 ()
                } ;
                JT2[0] =
                    pythia
                    .GetVector
                        (j2id[0])
                ; //
                JT2[1] =
                    pythia
                    .GetVector
                        (j2id[1])
                ; //
                long bid[2] = {
                    pythia.FindDaughter (
                        (long) /* ParentID = */ tid[0] ,
                        (long) /* DPID     = */ +5
                    ) ,
                    pythia.FindDaughter (
                        (long) /* ParentID = */ tid[1] ,
                        (long) /* DPID     = */ -5
                    )
                } ;
                BOT[0] =
                    pythia
                    .GetVector
                        (bid[0])
                ;
                BOT[1] =
                    pythia
                    .GetVector
                        (bid[1])
                ;
            }
            //
            fastjet::JetAlgorithm algo =
                fastjet::antikt_algorithm
            ; //
            fastjet::JetDefinition
                jet_def (
                    algo,
                    1.0
                )
            ;
            fastjet::ClusterSequence
                clust_seq (
                    hadrons,
                    jet_def
                )
            ;
            vector4s jets =
                clust_seq
                .inclusive_jets
                        (800.0)
            ;
            //
            if (jets.size()>0) {
                //
                bool genmatched =
                    false
                ; //
                for (size_t kk=0;kk<2;kk++) {
                    genmatched = genmatched || (
                        (jets[0].delta_R(TOP[kk])<0.6) &&
                        (jets[0].delta_R(WBS[kk])<0.6) &&
                        (jets[0].delta_R(BOT[kk])<0.6) &&
                        (jets[0].delta_R(JT1[kk])<0.6) &&
                        (jets[0].delta_R(JT2[kk])<0.6)
                    ) ;
                }
                //
                if (
                    (genmatched) &&
                    (jets[0].constituents().size()>2)
                ) {
                    auto tmppt =
                        jets[0]
                        .pt()
                    ; //
                    auto tmpet =
                        jets[0]
                        .rapidity()
                    ; //
                    if (
                        (
                            ( 800 < tmppt ) &&
                            ( tmppt < 900 )
                        ) && (
                            ( -2.5 < tmpet ) &&
                            ( tmpet < 2.5 )
                        )
                    ) {
                        writer(jets[0]);
                        count++ ;
                    }
                }
            }
        }
    }
    //
    inline void
    Generate_Train_Top () {
        CPPFileIO::ForkMe forker ;
        for (size_t i=0;i<8;i++)
        if (forker.InKid()) {
            Generate_TOP (
                i       ,
                "TRAIN" ,
                123     ,
                300000
            ) ; //
        }
    }
    //
    inline void
    Generate_Test_Top () {
        CPPFileIO::ForkMe forker ;
        for (size_t i=0;i<8;i++)
        if (forker.InKid()) {
            Generate_TOP (
                i      ,
                "TEST" ,
                456    ,
                50000
            ) ; //
        }
    }
    //
    inline void
    PlotNsub () {
        /* TRAIN */ if(true) {
            MyHistN <5,false>
                nsubcompare (
                    "TAU_TRAIN_TOP",
                    100,-0.01,100.01
                )
            ; //
            for(
                size_t index=0;
                index<8;
                index++
            ) {
                char tmp[256] ;
                sprintf (
                    tmp,
                    "./OUTS/TOP/TRAIN/%ld/nsub",
                    index
                ) ; //
                CPPFileIO::FullFileReader
                    <TYPE_NSub>
                        Reader (tmp)
                ; //
                for (size_t i=0;i<Reader();i++) {
                    nsubcompare.Fill<0>
                        (Reader(i)[0])
                    ; //
                    nsubcompare.Fill<1>
                        (Reader(i)[1])
                    ; //
                    nsubcompare.Fill<2>
                        (Reader(i)[2])
                    ; //
                    nsubcompare.Fill<3>
                        (Reader(i)[3])
                    ; //
                    nsubcompare.Fill<4>
                        (Reader(i)[4])
                    ; //
                }
            }
        }
        /* TEST */ if(true) {
            MyHistN <5,false>
                nsubcompare (
                    "TAU_TEST_TOP",
                    100,-0.01,100.01
                )
            ; //
            for(
                size_t index=0;
                index<8;
                index++
            ) {
                char tmp[256] ;
                sprintf (
                    tmp,
                    "./OUTS/TOP/TEST/%ld/nsub",
                    index
                ) ; //
                CPPFileIO::FullFileReader
                    <TYPE_NSub>
                        Reader (tmp)
                ; //
                for (size_t i=0;i<Reader();i++) {
                    nsubcompare.Fill<0>
                        (Reader(i)[0])
                    ; //
                    nsubcompare.Fill<1>
                        (Reader(i)[1])
                    ; //
                    nsubcompare.Fill<2>
                        (Reader(i)[2])
                    ; //
                    nsubcompare.Fill<3>
                        (Reader(i)[3])
                    ; //
                    nsubcompare.Fill<4>
                        (Reader(i)[4])
                    ; //
                }
            }
        }
    }
    //
    inline void
    PlotImages () {

        if(true) /* TRAIN */ {

            if(true) /* NORMAL */ {
                Plot2D tmp("TOP_TRAIN_IMAGES");
                tmp("./OUTS/TOP/TRAIN/0/image");
                tmp("./OUTS/TOP/TRAIN/1/image");
                tmp("./OUTS/TOP/TRAIN/2/image");
                tmp("./OUTS/TOP/TRAIN/3/image");
                tmp("./OUTS/TOP/TRAIN/4/image");
                tmp("./OUTS/TOP/TRAIN/5/image");
                tmp("./OUTS/TOP/TRAIN/6/image");
                tmp("./OUTS/TOP/TRAIN/7/image");
            }

            if(true) /* NOGRAMSCHMIDT */ {
                Plot2D tmp("TOP_TRAIN_IMAGES_NOGRAMSCHMIDT");
                tmp("./OUTS/TOP/TRAIN/0/NoGramSchmidtImageType");
                tmp("./OUTS/TOP/TRAIN/1/NoGramSchmidtImageType");
                tmp("./OUTS/TOP/TRAIN/2/NoGramSchmidtImageType");
                tmp("./OUTS/TOP/TRAIN/3/NoGramSchmidtImageType");
                tmp("./OUTS/TOP/TRAIN/4/NoGramSchmidtImageType");
                tmp("./OUTS/TOP/TRAIN/5/NoGramSchmidtImageType");
                tmp("./OUTS/TOP/TRAIN/6/NoGramSchmidtImageType");
                tmp("./OUTS/TOP/TRAIN/7/NoGramSchmidtImageType");
            }

            if(true) /* NOBOOST */ {
                Plot2D tmp("TOP_TRAIN_IMAGES_NOBOOST");
                tmp("./OUTS/TOP/TRAIN/0/UnBoostedimage");
                tmp("./OUTS/TOP/TRAIN/1/UnBoostedimage");
                tmp("./OUTS/TOP/TRAIN/2/UnBoostedimage");
                tmp("./OUTS/TOP/TRAIN/3/UnBoostedimage");
                tmp("./OUTS/TOP/TRAIN/4/UnBoostedimage");
                tmp("./OUTS/TOP/TRAIN/5/UnBoostedimage");
                tmp("./OUTS/TOP/TRAIN/6/UnBoostedimage");
                tmp("./OUTS/TOP/TRAIN/7/UnBoostedimage");
            }

        }

        if(true) /* TEST */ {

            if(true) /* NORMAL */ {
                Plot2D tmp("TOP_TEST_IMAGES");
                tmp("./OUTS/TOP/TEST/0/image");
                tmp("./OUTS/TOP/TEST/1/image");
                tmp("./OUTS/TOP/TEST/2/image");
                tmp("./OUTS/TOP/TEST/3/image");
                tmp("./OUTS/TOP/TEST/4/image");
                tmp("./OUTS/TOP/TEST/5/image");
                tmp("./OUTS/TOP/TEST/6/image");
                tmp("./OUTS/TOP/TEST/7/image");
            }

            if(true) /* NOGRAMSCHMIDT */ {
                Plot2D tmp("TOP_TEST_IMAGES_NOGRAMSCHMIDT");
                tmp("./OUTS/TOP/TEST/0/NoGramSchmidtImageType");
                tmp("./OUTS/TOP/TEST/1/NoGramSchmidtImageType");
                tmp("./OUTS/TOP/TEST/2/NoGramSchmidtImageType");
                tmp("./OUTS/TOP/TEST/3/NoGramSchmidtImageType");
                tmp("./OUTS/TOP/TEST/4/NoGramSchmidtImageType");
                tmp("./OUTS/TOP/TEST/5/NoGramSchmidtImageType");
                tmp("./OUTS/TOP/TEST/6/NoGramSchmidtImageType");
                tmp("./OUTS/TOP/TEST/7/NoGramSchmidtImageType");
            }

            if(true) /* NOBOOST */ {
                Plot2D tmp("TOP_TEST_IMAGES_NOBOOST");
                tmp("./OUTS/TOP/TEST/0/UnBoostedimage");
                tmp("./OUTS/TOP/TEST/1/UnBoostedimage");
                tmp("./OUTS/TOP/TEST/2/UnBoostedimage");
                tmp("./OUTS/TOP/TEST/3/UnBoostedimage");
                tmp("./OUTS/TOP/TEST/4/UnBoostedimage");
                tmp("./OUTS/TOP/TEST/5/UnBoostedimage");
                tmp("./OUTS/TOP/TEST/6/UnBoostedimage");
                tmp("./OUTS/TOP/TEST/7/UnBoostedimage");
            }

        }

    }
    //
}
namespace STEP3_B_GENERATEH4J {
    //
    using namespace MISC ;
    //
    inline void
    Generate (
        size_t      index                ,
        std::string MidName              ,
        size_t      random_seed = 123    ,
        size_t      N_Events    = 100000
    ) {
        CPPFileIO::SetCPUAffinity(index);
        std::string prefixname ;
        /* Make the directories: */ {
            std::string dirname = "./OUTS/" ;
            mkdir ( &(dirname[0]) , 0755 ) ;
            dirname = "./OUTS/H4J/" ;
            mkdir ( &(dirname[0]) , 0755 ) ;
            dirname = "./OUTS/H4J/" + MidName + "/" ;
            mkdir ( &(dirname[0]) , 0755 ) ;
            char tmp = '0' ;
            tmp = tmp + ( (char) index ) ;
            dirname = dirname + tmp + "/" ;
            mkdir ( &(dirname[0]) , 0755 ) ;
            prefixname = dirname ;
        }
		size_t count = 0 ;
		EventWriter writer (prefixname) ;
		MyPythia pythia ;
		/* Configure pythia: */ {
			pythia.readString ( "Beams:eCM = 13000"         ) ;
			pythia.readString ( "PhaseSpace:pTHatMin = 750" ) ;
			pythia.readString ( "PhaseSpace:pTHatMax = 950" ) ;
			pythia.readString ( "PartonLevel:MPI = off"     ) ;
			pythia.readString ( "Random:setSeed = on"       ) ;
			/* Set the random number Seed */ {
				char tmp[256] ;
				sprintf (
					tmp                 ,
					"Random:seed = %ld" ,
					index + random_seed
				) ; //
				pythia.readString
					(tmp)
				; //
			}
			pythia.readString ( "Higgs:useBSM = on"         ) ;
			pythia.readString ( "HiggsBSM:ffbar2H2Z = on"   ) ;
			pythia.readString ( "35:m0 = 180.0"             ) ;
			pythia.readString ( "35:onMode = off"           ) ;
			pythia.readString ( "35:onIfAny = 24 24"        ) ;
			pythia.readString ( "23:onMode = off"           ) ;
			pythia.readString ( "23:onIfAny = 12"           ) ;
			pythia.readString ( "24:onMode = off"           ) ;
			pythia.readString ( "24:onIfAny = 1 2 3 4"      ) ;
            pythia.init () ;
        }
        while (count<N_Events)
        if (pythia.next()) {
			vector4s
				hadrons
			; /* Prepare the list of hadrons: */ {
                for (
                    size_t i=0;
                    i<pythia.event.size();
                    i++
                ) if (
                    pythia.event[i]
                        .isFinal()
                ) {
                    int pid =
                        pythia
                        .event[i]
                        .id()
                    ;
                    if (pid<0) {pid=-pid;}
                    if (
                        ( pid != 12 ) &&
                        ( pid != 14 ) &&
                        ( pid != 16 )
                    ) {
                        vector4 tmp (
                            pythia.event[i].px () ,
                            pythia.event[i].py () ,
                            pythia.event[i].pz () ,
                            pythia.event[i].e  ()
                        ) ;
                        auto tmpeta =
                            tmp.rapidity()
                        ;
                        if (
                            ( -2.5     < tmpeta ) &&
                            ( tmpeta   < 2.5    ) &&
                            ( tmp.pt() > 0.5    )
                        ) {
                            hadrons.push_back
                                (tmp)
                            ; //
                        }
                    }
                }
            }
			//
			vector4
				HG2		,	WBS[2]	,
				JT1[2]	,	JT2[2]
			; /* Get These Vectors: */ {
				long hid =
					pythia.Recurse (
						pythia.FindParticle ( 35 )
					)
				; //
				HG2 = pythia.GetVector (hid) ;
				long wid[2] = {
					pythia.Recurse ( pythia.FindDaughter ( (long) /*ParentID=*/ hid , (long) /*DPID=*/  24 ) ) ,
					pythia.Recurse ( pythia.FindDaughter ( (long) /*ParentID=*/ hid , (long) /*DPID=*/ -24 ) )
				} ; {
					WBS[0] = pythia.GetVector (wid[0]) ;
					WBS[1] = pythia.GetVector (wid[1]) ;
				}
				long j1id[2] = {
					pythia.event[wid[0]].daughter1 () ,
					pythia.event[wid[1]].daughter1 ()
				} ; {
					JT1[0] = pythia.GetVector (j1id[0]) ;
					JT1[1] = pythia.GetVector (j1id[1]) ;
				}
				long j2id[2] = {
					pythia.event[wid[0]].daughter2 () ,
					pythia.event[wid[1]].daughter2 ()
				} ; {
					JT2[0] = pythia.GetVector (j2id[0]) ;
					JT2[1] = pythia.GetVector (j2id[1]) ;
				}
			}
            //
            fastjet::JetAlgorithm algo =
                fastjet::antikt_algorithm
            ; //
            fastjet::JetDefinition
                jet_def (
                    algo,
                    1.0
                )
            ;
            fastjet::ClusterSequence
                clust_seq (
                    hadrons,
                    jet_def
                )
            ;
            vector4s jets =
                clust_seq
                .inclusive_jets
                        (800.0)
            ;
            //
			if (jets.size()>0) {
				//
				bool genmatched =
					(
						( jets[0].delta_R (HG2)		< 0.6 )	&&
						( jets[0].delta_R (WBS[0])	< 0.6 )	&&
						( jets[0].delta_R (WBS[1])	< 0.6 )	&&
						( jets[0].delta_R (JT1[0])	< 0.6 )	&&
						( jets[0].delta_R (JT1[1])	< 0.6 )	&&
						( jets[0].delta_R (JT2[0])	< 0.6 )	&&
						( jets[0].delta_R (JT2[1])	< 0.6 )
					)
				; //
                if (
                    (genmatched) &&
                    (jets[0].constituents().size()>2)
                ) {
                    auto tmppt =
                        jets[0]
                        .pt()
                    ; //
                    auto tmpet =
                        jets[0]
                        .rapidity()
                    ; //
                    if (
                        (
                            ( 800 < tmppt ) &&
                            ( tmppt < 900 )
                        ) && (
                            ( -2.5 < tmpet ) &&
                            ( tmpet < 2.5 )
                        )
                    ) {
                        writer(jets[0]);
                        count++ ;
                    }
                }
            }
        }
    }
    //
	inline void
	Generate_Train () {
        CPPFileIO::ForkMe forker ;
        for (size_t i=0;i<8;i++)
        if (forker.InKid()) {
            Generate (
                i       ,
                "TRAIN" ,
                123     ,
                300000
            ) ; //
        }
    }
    //
    inline void
    Generate_Test () {
        CPPFileIO::ForkMe forker ;
        for (size_t i=0;i<8;i++)
        if (forker.InKid()) {
            Generate (
                i      ,
                "TEST" ,
                456    ,
                50000
            ) ; //
        }
    }
    //
    inline void
    PlotNsub () {
        /* TRAIN */ if(true) {
            MyHistN <5,false>
                nsubcompare (
                    "TAU_TRAIN_H4J",
                    100,-0.01,100.01
                )
            ; //
            for(
                size_t index=0;
                index<8;
                index++
            ) {
                char tmp[256] ;
                sprintf (
                    tmp,
                    "./OUTS/H4J/TRAIN/%ld/nsub",
                    index
                ) ; //
                CPPFileIO::FullFileReader
                    <TYPE_NSub>
                        Reader (tmp)
                ; //
                for (size_t i=0;i<Reader();i++) {
                    nsubcompare.Fill<0>
                        (Reader(i)[0])
                    ; //
                    nsubcompare.Fill<1>
                        (Reader(i)[1])
                    ; //
                    nsubcompare.Fill<2>
                        (Reader(i)[2])
                    ; //
                    nsubcompare.Fill<3>
                        (Reader(i)[3])
                    ; //
                    nsubcompare.Fill<4>
                        (Reader(i)[4])
                    ; //
                }
            }
        }
        /* TEST */ if(true) {
            MyHistN <5,false>
                nsubcompare (
                    "TAU_TEST_H4J",
                    100,-0.01,100.01
                )
            ; //
            for(
                size_t index=0;
                index<8;
                index++
            ) {
                char tmp[256] ;
                sprintf (
                    tmp,
                    "./OUTS/H4J/TEST/%ld/nsub",
                    index
                ) ; //
                CPPFileIO::FullFileReader
                    <TYPE_NSub>
                        Reader (tmp)
                ; //
                for (size_t i=0;i<Reader();i++) {
                    nsubcompare.Fill<0>
                        (Reader(i)[0])
                    ; //
                    nsubcompare.Fill<1>
                        (Reader(i)[1])
                    ; //
                    nsubcompare.Fill<2>
                        (Reader(i)[2])
                    ; //
                    nsubcompare.Fill<3>
                        (Reader(i)[3])
                    ; //
                    nsubcompare.Fill<4>
                        (Reader(i)[4])
                    ; //
                }
            }
        }
    }
    //
    inline void
    PlotImages () {

        if(true) /* TRAIN */ {

            if(true) /* NORMAL */ {
                Plot2D tmp("H4J_TRAIN_IMAGES");
                tmp("./OUTS/H4J/TRAIN/0/image");
                tmp("./OUTS/H4J/TRAIN/1/image");
                tmp("./OUTS/H4J/TRAIN/2/image");
                tmp("./OUTS/H4J/TRAIN/3/image");
                tmp("./OUTS/H4J/TRAIN/4/image");
                tmp("./OUTS/H4J/TRAIN/5/image");
                tmp("./OUTS/H4J/TRAIN/6/image");
                tmp("./OUTS/H4J/TRAIN/7/image");
            }

            if(true) /* NOGRAMSCHMIDT */ {
                Plot2D tmp("H4J_TRAIN_IMAGES_NOGRAMSCHMIDT");
                tmp("./OUTS/H4J/TRAIN/0/NoGramSchmidtImageType");
                tmp("./OUTS/H4J/TRAIN/1/NoGramSchmidtImageType");
                tmp("./OUTS/H4J/TRAIN/2/NoGramSchmidtImageType");
                tmp("./OUTS/H4J/TRAIN/3/NoGramSchmidtImageType");
                tmp("./OUTS/H4J/TRAIN/4/NoGramSchmidtImageType");
                tmp("./OUTS/H4J/TRAIN/5/NoGramSchmidtImageType");
                tmp("./OUTS/H4J/TRAIN/6/NoGramSchmidtImageType");
                tmp("./OUTS/H4J/TRAIN/7/NoGramSchmidtImageType");
            }

            if(true) /* NOBOOST */ {
                Plot2D tmp("H4J_TRAIN_IMAGES_NOBOOST");
                tmp("./OUTS/H4J/TRAIN/0/UnBoostedimage");
                tmp("./OUTS/H4J/TRAIN/1/UnBoostedimage");
                tmp("./OUTS/H4J/TRAIN/2/UnBoostedimage");
                tmp("./OUTS/H4J/TRAIN/3/UnBoostedimage");
                tmp("./OUTS/H4J/TRAIN/4/UnBoostedimage");
                tmp("./OUTS/H4J/TRAIN/5/UnBoostedimage");
                tmp("./OUTS/H4J/TRAIN/6/UnBoostedimage");
                tmp("./OUTS/H4J/TRAIN/7/UnBoostedimage");
            }

        }

        if(true) /* TEST */ {

            if(true) /* NORMAL */ {
                Plot2D tmp("H4J_TEST_IMAGES");
                tmp("./OUTS/H4J/TEST/0/image");
                tmp("./OUTS/H4J/TEST/1/image");
                tmp("./OUTS/H4J/TEST/2/image");
                tmp("./OUTS/H4J/TEST/3/image");
                tmp("./OUTS/H4J/TEST/4/image");
                tmp("./OUTS/H4J/TEST/5/image");
                tmp("./OUTS/H4J/TEST/6/image");
                tmp("./OUTS/H4J/TEST/7/image");
            }

            if(true) /* NOGRAMSCHMIDT */ {
                Plot2D tmp("H4J_TEST_IMAGES_NOGRAMSCHMIDT");
                tmp("./OUTS/H4J/TEST/0/NoGramSchmidtImageType");
                tmp("./OUTS/H4J/TEST/1/NoGramSchmidtImageType");
                tmp("./OUTS/H4J/TEST/2/NoGramSchmidtImageType");
                tmp("./OUTS/H4J/TEST/3/NoGramSchmidtImageType");
                tmp("./OUTS/H4J/TEST/4/NoGramSchmidtImageType");
                tmp("./OUTS/H4J/TEST/5/NoGramSchmidtImageType");
                tmp("./OUTS/H4J/TEST/6/NoGramSchmidtImageType");
                tmp("./OUTS/H4J/TEST/7/NoGramSchmidtImageType");
            }

            if(true) /* NOBOOST */ {
                Plot2D tmp("H4J_TEST_IMAGES_NOBOOST");
                tmp("./OUTS/H4J/TEST/0/UnBoostedimage");
                tmp("./OUTS/H4J/TEST/1/UnBoostedimage");
                tmp("./OUTS/H4J/TEST/2/UnBoostedimage");
                tmp("./OUTS/H4J/TEST/3/UnBoostedimage");
                tmp("./OUTS/H4J/TEST/4/UnBoostedimage");
                tmp("./OUTS/H4J/TEST/5/UnBoostedimage");
                tmp("./OUTS/H4J/TEST/6/UnBoostedimage");
                tmp("./OUTS/H4J/TEST/7/UnBoostedimage");
            }

        }

    }
    //
}
////////////////////////////////////////////////////////////////
namespace STEP4_MIX_DATA {
    //
    using namespace MISC ;
    //
    inline void
    DataMixer (
        std::string src1	,
        std::string src2	,
        std::string dst
    ) {
        CPPFileIO::FileVector
            <imagetype>
                outimage (
                    dst +
                    "/image"
                )
        ;
        CPPFileIO::FileVector
            <TYPE_DATA>
                outlabel (
                    dst +
                    "/label"
                )
        ;
        CPPFileIO::FullFileReader
            <imagetype>
                SigRead (
                    src1 +
                    "/image"
                )
        ; //
        CPPFileIO::FullFileReader
            <imagetype>
                BkgRead (
                    src2 +
                    "/image"
                )
        ; //
        size_t Limit =
            CPPFileIO::mymin (
                BkgRead() ,
                SigRead()
            )
        ;
        for(size_t i=0;i<Limit;i++){
            outimage.push_back
                (SigRead(i))
            ;
            outlabel.push_back
                (1.0)
            ;
            outimage.push_back
                (BkgRead(i))
            ;
            outlabel.push_back
                (0.0)
            ;
        }
    }
    //
    inline void
    MixDataTopQCD () {
        /* Create directories: */ {
            mkdir (
                "./OUTS/TMP/"               ,
                0755
            ) ; //
            mkdir (
                "./OUTS/TMP/QCD_TOP/"       ,
                0755
            ) ; //
            mkdir (
                "./OUTS/TMP/QCD_TOP/TRAIN/" ,
                0755
            ) ; //
            mkdir (
                "./OUTS/TMP/QCD_TOP/TEST/"  ,
                0755
            ) ; //
        }
        for (size_t index=0;index<8;index++) {
            /* Train Part : */ if ( true ) {
                char
                    tmp1[512]
                ; /* prepare the destination: */ {
                    sprintf (
                        tmp1,
                        "./OUTS/TMP/QCD_TOP/TRAIN/%ld/",
                        index
                    ) ; //
                }
                mkdir
                    (tmp1,0755)
                ; //
                char
                    tmp2[512], tmp3[512]
                ; /* prepare the sources: */ {
                    sprintf(tmp2,"./OUTS/TOP/TRAIN/%ld/",index);
                    sprintf(tmp3,"./OUTS/QCD/TRAIN/%ld/",index);
                }
                DataMixer
                    (tmp2,tmp3,tmp1)
                ; //
            }
            /* Test  Part : */ if ( true ) {
                char
                    tmp1[512]
                ; /* prepare the destination: */ {
                    sprintf (
                        tmp1,
                        "./OUTS/TMP/QCD_TOP/TEST/%ld/",
                        index
                    ) ; //
                }
                mkdir
                    (tmp1,0755)
                ; //
                char
                    tmp2[512], tmp3[512]
                ; /* prepare the sources: */ {
                    sprintf(tmp2,"./OUTS/TOP/TEST/%ld/",index);
                    sprintf(tmp3,"./OUTS/QCD/TEST/%ld/",index);
                }
                DataMixer
                    (tmp2,tmp3,tmp1)
                ; //
            }
        }
    }
    //
    inline void
    MixDataWBSQCD () {
        /* Create directories: */ {
            mkdir (
                "./OUTS/TMP/"               ,
                0755
            ) ; //
            mkdir (
                "./OUTS/TMP/QCD_WBS/"       ,
                0755
            ) ; //
            mkdir (
                "./OUTS/TMP/QCD_WBS/TRAIN/" ,
                0755
            ) ; //
            mkdir (
                "./OUTS/TMP/QCD_WBS/TEST/"  ,
                0755
            ) ; //
        }
        for (size_t index=0;index<8;index++) {
            /* Train Part : */ if ( true ) {
                char
                    tmp1[512]
                ; /* prepare the destination: */ {
                    sprintf (
                        tmp1,
                        "./OUTS/TMP/QCD_WBS/TRAIN/%ld/",
                        index
                    ) ; //
                    mkdir (
                        tmp1,
                        0755
                    ) ; //
                }
                char
                    tmp2[512], tmp3[512]
                ; /* prepare the sources: */ {
                    sprintf(
                        tmp2,
                        "./OUTS/WBS/TRAIN/%ld/",
                        index
                    ) ;
                    sprintf(
                        tmp3,
                        "./OUTS/QCD/TRAIN/%ld/",
                        index
                    );
                }
                DataMixer
                    (tmp2,tmp3,tmp1)
                ; //
            }
            /* Test  Part : */ if ( true ) {
                char
                    tmp1[512]
                ; /* prepare the destination: */ {
                    sprintf (
                        tmp1,
                        "./OUTS/TMP/QCD_WBS/TEST/%ld/",
                        index
                    ) ; //
                    mkdir (
                        tmp1,
                        0755
                    ) ; //
                }
                char
                    tmp2[512], tmp3[512]
                ; /* prepare the sources: */ {
                    sprintf(tmp2,"./OUTS/WBS/TEST/%ld/",index);
                    sprintf(tmp3,"./OUTS/QCD/TEST/%ld/",index);
                }
                DataMixer
                    (tmp2,tmp3,tmp1)
                ; //
            }
        }
    }
    //
}
////////////////////////////////////////////////////////////////
namespace STEP5_PLOTLOSSES_TMVAROOT {
    using namespace
        MISC
    ; //
    //
    inline void
    PlotLosses () {
		MyHistN <2,false>
			Hists (
				"Losses",
				100,-0.01,0.41
			)
		; //
        for(size_t i=0;i<8;i++){
            char tmp[512] ;
            /* QCD TRAIN */ if(true) {
                sprintf (
                    tmp,
                    "./OUTS/QCD/TRAIN/%ld/predict",
                    i
                ) ; //
                CPPFileIO::FullFileReader
                    <TYPE_DATA>
                        Reader (
                            tmp
                        )
                ; //
                TYPE_DATA Sumloss = 0 ;
                for(size_t j=0;j<Reader();j++){
                    Sumloss +=
                        Reader(j)
                    ; //
                    Hists.Fill<0>(Reader(j));
                }
                printf(
                    "Final Sum = %e\n",
                    Sumloss /
                    ((TYPE_DATA)Reader()))
                ; //
            }
            /* QCD TEST */ if(true) {
                sprintf (
                    tmp,
                    "./OUTS/QCD/TEST/%ld/predict",
                    i
                ) ; //
                CPPFileIO::FullFileReader
                    <TYPE_DATA>
                        Reader (
                            tmp
                        )
                ; //
                TYPE_DATA Sumloss = 0 ;
                for(size_t j=0;j<Reader();j++){
                    Sumloss +=
                        Reader(j)
                    ; //
                    Hists.Fill<1>(Reader(j));
                }
                printf(
                    "Final Sum = %e\n",
                    Sumloss /
                    ((TYPE_DATA)Reader()))
                ; //
            }
            /* TOP TRAIN */ if(false) {
                sprintf (
                    tmp,
                    "./OUTS/TOP/TRAIN/%ld/predict",
                    i
                ) ; //
                CPPFileIO::FullFileReader
                    <TYPE_DATA>
                        Reader (
                            tmp
                        )
                ; //
                TYPE_DATA Sumloss = 0 ;
                for(size_t j=0;j<Reader();j++){
                    Sumloss +=
                        Reader(j)
                    ; //
                    Hists.Fill<1>(Reader(j));
                }
                printf(
                    "Final Sum = %e\n",
                    Sumloss /
                    ((TYPE_DATA)Reader()))
                ; //
            }
            /* TOP TEST */ if(false) {
                sprintf (
                    tmp,
                    "./OUTS/TOP/TEST/%ld/predict",
                    i
                ) ; //
                CPPFileIO::FullFileReader
                    <TYPE_DATA>
                        Reader (
                            tmp
                        )
                ; //
                TYPE_DATA Sumloss = 0 ;
                for(size_t j=0;j<Reader();j++){
                    Sumloss +=
                        Reader(j)
                    ; //
                    Hists.Fill<3>(Reader(j));
                }
                printf(
                    "Final Sum = %e\n",
                    Sumloss /
                    ((TYPE_DATA)Reader()))
                ; //
            }
        }
    }
    //
    inline void
    PlotLosses_CNN () {
		MyHistN <2,false>
			Hists (
				"Losses_CNN",
				100,-0.01,0.41
			)
		; //
        for(size_t i=0;i<8;i++){
            char tmp[512] ;
            /* QCD TRAIN */ if(true) {
                sprintf (
                    tmp,
                    "./OUTS/QCD/TRAIN/%ld/loss_cnn",
                    i
                ) ; //
                CPPFileIO::FullFileReader
                    <TYPE_DATA>
                        Reader (
                            tmp
                        )
                ; //
                TYPE_DATA Sumloss = 0 ;
                for(size_t j=0;j<Reader();j++){
                    Sumloss +=
                        Reader(j)
                    ; //
                    Hists.Fill<0>(Reader(j));
                }
                printf(
                    "Final Sum = %e\n",
                    Sumloss /
                    ((TYPE_DATA)Reader()))
                ; //
            }
            /* QCD TEST */ if(true) {
                sprintf (
                    tmp,
                    "./OUTS/QCD/TEST/%ld/loss_cnn",
                    i
                ) ; //
                CPPFileIO::FullFileReader
                    <TYPE_DATA>
                        Reader (
                            tmp
                        )
                ; //
                TYPE_DATA Sumloss = 0 ;
                for(size_t j=0;j<Reader();j++){
                    Sumloss +=
                        Reader(j)
                    ; //
                    Hists.Fill<1>(Reader(j));
                }
                printf(
                    "Final Sum = %e\n",
                    Sumloss /
                    ((TYPE_DATA)Reader()))
                ; //
            }
            /* TOP TRAIN */ if(false) {
                sprintf (
                    tmp,
                    "./OUTS/TOP/TRAIN/%ld/loss_cnn",
                    i
                ) ; //
                CPPFileIO::FullFileReader
                    <TYPE_DATA>
                        Reader (
                            tmp
                        )
                ; //
                TYPE_DATA Sumloss = 0 ;
                for(size_t j=0;j<Reader();j++){
                    Sumloss +=
                        Reader(j)
                    ; //
                    Hists.Fill<1>(Reader(j));
                }
                printf(
                    "Final Sum = %e\n",
                    Sumloss /
                    ((TYPE_DATA)Reader()))
                ; //
            }
            /* TOP TEST */ if(false) {
                sprintf (
                    tmp,
                    "./OUTS/TOP/TEST/%ld/loss_cnn",
                    i
                ) ; //
                CPPFileIO::FullFileReader
                    <TYPE_DATA>
                        Reader (
                            tmp
                        )
                ; //
                TYPE_DATA Sumloss = 0 ;
                for(size_t j=0;j<Reader();j++){
                    Sumloss +=
                        Reader(j)
                    ; //
                    Hists.Fill<3>(Reader(j));
                }
                printf(
                    "Final Sum = %e\n",
                    Sumloss /
                    ((TYPE_DATA)Reader()))
                ; //
            }
        }
    }
    //
    inline void
    PlotLosses_HPT () {
		MyHistN <4,true>
			Hists (
				"Losses_HPT",
				100,
				-0.01,0.41
			)
		; //
        for(size_t i=0;i<8;i++){
            char tmp[512] ;
            /* QCD TRAIN */ if(true) {
                sprintf (
                    tmp,
                    "./OUTS/QCD_HPT/TRAIN/%ld/predict",
                    i
                ) ; //
                CPPFileIO::FullFileReader
                    <TYPE_DATA>
                        Reader (
                            tmp
                        )
                ; //
                TYPE_DATA Sumloss = 0 ;
                for(size_t j=0;j<Reader();j++){
                    Sumloss +=
                        Reader(j)
                    ; //
                    Hists.Fill<0>(Reader(j));
                }
                printf(
                    "Final Sum = %e\n",
                    Sumloss /
                    ((TYPE_DATA)Reader()))
                ; //
            }
            /* QCD TEST */ if(true) {
                sprintf (
                    tmp,
                    "./OUTS/QCD_HPT/TEST/%ld/predict",
                    i
                ) ; //
                CPPFileIO::FullFileReader
                    <TYPE_DATA>
                        Reader (
                            tmp
                        )
                ; //
                TYPE_DATA Sumloss = 0 ;
                for(size_t j=0;j<Reader();j++){
                    Sumloss +=
                        Reader(j)
                    ; //
                    Hists.Fill<2>(Reader(j));
                }
                printf(
                    "Final Sum = %e\n",
                    Sumloss /
                    ((TYPE_DATA)Reader()))
                ; //
            }
            /* TOP TRAIN */ if(true) {
                sprintf (
                    tmp,
                    "./OUTS/TOP/TRAIN/%ld/predict",
                    i
                ) ; //
                CPPFileIO::FullFileReader
                    <TYPE_DATA>
                        Reader (
                            tmp
                        )
                ; //
                TYPE_DATA Sumloss = 0 ;
                for(size_t j=0;j<Reader();j++){
                    Sumloss +=
                        Reader(j)
                    ; //
                    Hists.Fill<1>(Reader(j));
                }
                printf(
                    "Final Sum = %e\n",
                    Sumloss /
                    ((TYPE_DATA)Reader()))
                ; //
            }
            /* TOP TEST */ if(true) {
                sprintf (
                    tmp,
                    "./OUTS/TOP/TEST/%ld/predict",
                    i
                ) ; //
                CPPFileIO::FullFileReader
                    <TYPE_DATA>
                        Reader (
                            tmp
                        )
                ; //
                TYPE_DATA Sumloss = 0 ;
                for(size_t j=0;j<Reader();j++){
                    Sumloss +=
                        Reader(j)
                    ; //
                    Hists.Fill<3>(Reader(j));
                }
                printf(
                    "Final Sum = %e\n",
                    Sumloss /
                    ((TYPE_DATA)Reader()))
                ; //
            }
        }
    }
    //
	inline void
	MakeTMVARoot () {

		/* Create the directory structure: */ {
			mkdir ( "./OUTS/"			, 0755 ) ;
			mkdir ( "./OUTS/TMP/"		, 0755 ) ;
			mkdir ( "./OUTS/TMP/tmva/"	, 0755 ) ;
		}

		TFile
			outfile (
				"./OUTS/TMP/OUT_SHORT_FC_AE.root",
				"RECREATE"
			)
		; //
		TTree signalTree ("TreeS","TreeS") ;
		TTree background ("TreeB","TreeB") ;

		float
			mass	,
			loss	,
			nsub[5]
		;

		signalTree.Branch ( "mass"	, & mass	) ;
		signalTree.Branch ( "loss"	, & loss	) ;
		signalTree.Branch ( "nsub0"	, & nsub[0]	) ;
		signalTree.Branch ( "nsub1"	, & nsub[2]	) ;
		signalTree.Branch ( "nsub2"	, & nsub[3]	) ;
		signalTree.Branch ( "nsub3"	, & nsub[3]	) ;
		signalTree.Branch ( "nsub4"	, & nsub[4]	) ;

		background.Branch ( "mass"	, & mass	) ;
		background.Branch ( "loss"	, & loss	) ;
		background.Branch ( "nsub0"	, & nsub[0]	) ;
		background.Branch ( "nsub1"	, & nsub[2]	) ;
		background.Branch ( "nsub2"	, & nsub[3]	) ;
		background.Branch ( "nsub3"	, & nsub[3]	) ;
		background.Branch ( "nsub4"	, & nsub[4]	) ;

        /* QCD TEST */ {
            names lossfiles ;
			/* Create the loss list */ {
                lossfiles.push_back ("./OUTS/QCD/TEST/0/predict") ;
                lossfiles.push_back ("./OUTS/QCD/TEST/1/predict") ;
                lossfiles.push_back ("./OUTS/QCD/TEST/2/predict") ;
                lossfiles.push_back ("./OUTS/QCD/TEST/3/predict") ;
                lossfiles.push_back ("./OUTS/QCD/TEST/4/predict") ;
                lossfiles.push_back ("./OUTS/QCD/TEST/5/predict") ;
                lossfiles.push_back ("./OUTS/QCD/TEST/6/predict") ;
                lossfiles.push_back ("./OUTS/QCD/TEST/7/predict") ;
            }
            names vectfiles ;
			/* Create the vector list: */ {
                vectfiles.push_back ("./OUTS/QCD/TEST/0/vector") ;
                vectfiles.push_back ("./OUTS/QCD/TEST/1/vector") ;
                vectfiles.push_back ("./OUTS/QCD/TEST/2/vector") ;
                vectfiles.push_back ("./OUTS/QCD/TEST/3/vector") ;
                vectfiles.push_back ("./OUTS/QCD/TEST/4/vector") ;
                vectfiles.push_back ("./OUTS/QCD/TEST/5/vector") ;
                vectfiles.push_back ("./OUTS/QCD/TEST/6/vector") ;
                vectfiles.push_back ("./OUTS/QCD/TEST/7/vector") ;
            }
			names nsubfiles ;
			/* Get list for nsub */ {
				nsubfiles.push_back("./OUTS/QCD/TEST/0/nsub");
				nsubfiles.push_back("./OUTS/QCD/TEST/1/nsub");
				nsubfiles.push_back("./OUTS/QCD/TEST/2/nsub");
				nsubfiles.push_back("./OUTS/QCD/TEST/3/nsub");
				nsubfiles.push_back("./OUTS/QCD/TEST/4/nsub");
				nsubfiles.push_back("./OUTS/QCD/TEST/5/nsub");
				nsubfiles.push_back("./OUTS/QCD/TEST/6/nsub");
				nsubfiles.push_back("./OUTS/QCD/TEST/7/nsub");
			}
			for(size_t index=0;index<8;index++){
				READER_Loss
					READER_LOSS		(
						lossfiles[index]
					)
				; //
				READER_Vector
					READER_VECTOR	(
						vectfiles[index]
					)
				; //
				READER_NSub
					READER_NSUB		(
						nsubfiles[index]
					)
				; //
				for(
					size_t i = 0 ;
					i < READER_LOSS() ;
					i++
				) {
					loss	= READER_LOSS(i)		;
					mass	= READER_VECTOR(i).m()	;
					nsub[0]	= READER_NSUB(i)[0]		;
					nsub[1]	= READER_NSUB(i)[1]		;
					nsub[2]	= READER_NSUB(i)[2]		;
					nsub[3]	= READER_NSUB(i)[3]		;
					nsub[4]	= READER_NSUB(i)[4]		;
					background.Fill();
				}
			}
		}
		/* TOP TEST */ {
			names lossfiles ;
			/* Create the loss list */ {
				lossfiles.push_back ("./OUTS/TOP/TEST/0/predict") ;
				lossfiles.push_back ("./OUTS/TOP/TEST/1/predict") ;
				lossfiles.push_back ("./OUTS/TOP/TEST/2/predict") ;
				lossfiles.push_back ("./OUTS/TOP/TEST/3/predict") ;
				lossfiles.push_back ("./OUTS/TOP/TEST/4/predict") ;
				lossfiles.push_back ("./OUTS/TOP/TEST/5/predict") ;
				lossfiles.push_back ("./OUTS/TOP/TEST/6/predict") ;
				lossfiles.push_back ("./OUTS/TOP/TEST/7/predict") ;
			}
			names vectfiles ;
			/* Create the vector list: */ {
				vectfiles.push_back ("./OUTS/TOP/TEST/0/vector") ;
				vectfiles.push_back ("./OUTS/TOP/TEST/1/vector") ;
				vectfiles.push_back ("./OUTS/TOP/TEST/2/vector") ;
				vectfiles.push_back ("./OUTS/TOP/TEST/3/vector") ;
				vectfiles.push_back ("./OUTS/TOP/TEST/4/vector") ;
				vectfiles.push_back ("./OUTS/TOP/TEST/5/vector") ;
				vectfiles.push_back ("./OUTS/TOP/TEST/6/vector") ;
				vectfiles.push_back ("./OUTS/TOP/TEST/7/vector") ;
			}
			names nsubfiles ;
			/* Get list for nsub */ {
				nsubfiles.push_back("./OUTS/TOP/TEST/0/nsub");
				nsubfiles.push_back("./OUTS/TOP/TEST/1/nsub");
				nsubfiles.push_back("./OUTS/TOP/TEST/2/nsub");
				nsubfiles.push_back("./OUTS/TOP/TEST/3/nsub");
				nsubfiles.push_back("./OUTS/TOP/TEST/4/nsub");
				nsubfiles.push_back("./OUTS/TOP/TEST/5/nsub");
				nsubfiles.push_back("./OUTS/TOP/TEST/6/nsub");
				nsubfiles.push_back("./OUTS/TOP/TEST/7/nsub");
			}
			for(size_t index=0;index<8;index++){
				READER_Loss
					READER_LOSS		(
						lossfiles[index]
					)
				; //
				READER_Vector
					READER_VECTOR	(
						vectfiles[index]
					)
				; //
				READER_NSub
					READER_NSUB		(
						nsubfiles[index]
					)
				; //
				for(
					size_t i = 0 ;
					i < READER_LOSS() ;
					i++
				) {
					loss	= READER_LOSS(i)		;
					mass	= READER_VECTOR(i).m()	;
					nsub[0]	= READER_NSUB(i)[0]		;
					nsub[1]	= READER_NSUB(i)[1]		;
					nsub[2]	= READER_NSUB(i)[2]		;
					nsub[3]	= READER_NSUB(i)[3]		;
					nsub[4]	= READER_NSUB(i)[4]		;
					signalTree.Fill();
				}
			}
		}
		signalTree.Write();
		background.Write();
    }
    //
	inline void
	MakeTMVARoot_HUGE () {

		/* Create the directory structure: */ {
			mkdir ( "./OUTS/"			, 0755 ) ;
			mkdir ( "./OUTS/TMP/"		, 0755 ) ;
			mkdir ( "./OUTS/TMP/tmva/"	, 0755 ) ;
		}

		TFile
			outfile (
				"./OUTS/TMP/OUT_HUGE_FC_AE.root",
				"RECREATE"
			)
		; //
		TTree signalTree ("TreeS","TreeS") ;
		TTree background ("TreeB","TreeB") ;

		float
			mass	,
			loss	,
			nsub[5]
		;

		signalTree.Branch ( "mass"	, & mass	) ;
		signalTree.Branch ( "loss"	, & loss	) ;
		signalTree.Branch ( "nsub0"	, & nsub[0]	) ;
		signalTree.Branch ( "nsub1"	, & nsub[2]	) ;
		signalTree.Branch ( "nsub2"	, & nsub[3]	) ;
		signalTree.Branch ( "nsub3"	, & nsub[3]	) ;
		signalTree.Branch ( "nsub4"	, & nsub[4]	) ;

		background.Branch ( "mass"	, & mass	) ;
		background.Branch ( "loss"	, & loss	) ;
		background.Branch ( "nsub0"	, & nsub[0]	) ;
		background.Branch ( "nsub1"	, & nsub[2]	) ;
		background.Branch ( "nsub2"	, & nsub[3]	) ;
		background.Branch ( "nsub3"	, & nsub[3]	) ;
		background.Branch ( "nsub4"	, & nsub[4]	) ;

		/* QCD TEST */ {
			names lossfiles ;
			/* Create the loss list */ {
				lossfiles.push_back ("./OUTS/QCD/TEST/0/predict_huge") ;
				lossfiles.push_back ("./OUTS/QCD/TEST/1/predict_huge") ;
				lossfiles.push_back ("./OUTS/QCD/TEST/2/predict_huge") ;
				lossfiles.push_back ("./OUTS/QCD/TEST/3/predict_huge") ;
				lossfiles.push_back ("./OUTS/QCD/TEST/4/predict_huge") ;
				lossfiles.push_back ("./OUTS/QCD/TEST/5/predict_huge") ;
				lossfiles.push_back ("./OUTS/QCD/TEST/6/predict_huge") ;
				lossfiles.push_back ("./OUTS/QCD/TEST/7/predict_huge") ;
			}
			names vectfiles ;
			/* Create the vector list: */ {
				vectfiles.push_back ("./OUTS/QCD/TEST/0/vector") ;
				vectfiles.push_back ("./OUTS/QCD/TEST/1/vector") ;
				vectfiles.push_back ("./OUTS/QCD/TEST/2/vector") ;
				vectfiles.push_back ("./OUTS/QCD/TEST/3/vector") ;
				vectfiles.push_back ("./OUTS/QCD/TEST/4/vector") ;
				vectfiles.push_back ("./OUTS/QCD/TEST/5/vector") ;
				vectfiles.push_back ("./OUTS/QCD/TEST/6/vector") ;
				vectfiles.push_back ("./OUTS/QCD/TEST/7/vector") ;
			}
			names nsubfiles ;
			/* Get list for nsub */ {
				nsubfiles.push_back("./OUTS/QCD/TEST/0/nsub");
				nsubfiles.push_back("./OUTS/QCD/TEST/1/nsub");
				nsubfiles.push_back("./OUTS/QCD/TEST/2/nsub");
				nsubfiles.push_back("./OUTS/QCD/TEST/3/nsub");
				nsubfiles.push_back("./OUTS/QCD/TEST/4/nsub");
				nsubfiles.push_back("./OUTS/QCD/TEST/5/nsub");
				nsubfiles.push_back("./OUTS/QCD/TEST/6/nsub");
				nsubfiles.push_back("./OUTS/QCD/TEST/7/nsub");
			}
			for(size_t index=0;index<8;index++){
				READER_Loss
					READER_LOSS		(
						lossfiles[index]
					)
				; //
				READER_Vector
					READER_VECTOR	(
						vectfiles[index]
					)
				; //
				READER_NSub
					READER_NSUB		(
						nsubfiles[index]
					)
				; //
				for(
					size_t i = 0 ;
					i < READER_LOSS() ;
					i++
				) {
					loss	= READER_LOSS(i)		;
					mass	= READER_VECTOR(i).m()	;
					nsub[0]	= READER_NSUB(i)[0]		;
					nsub[1]	= READER_NSUB(i)[1]		;
					nsub[2]	= READER_NSUB(i)[2]		;
					nsub[3]	= READER_NSUB(i)[3]		;
					nsub[4]	= READER_NSUB(i)[4]		;
					background.Fill();
				}
			}
		}
		/* TOP TEST */ {
			names lossfiles ;
			/* Create the loss list */ {
				lossfiles.push_back ("./OUTS/TOP/TEST/0/predict_huge") ;
				lossfiles.push_back ("./OUTS/TOP/TEST/1/predict_huge") ;
				lossfiles.push_back ("./OUTS/TOP/TEST/2/predict_huge") ;
				lossfiles.push_back ("./OUTS/TOP/TEST/3/predict_huge") ;
				lossfiles.push_back ("./OUTS/TOP/TEST/4/predict_huge") ;
				lossfiles.push_back ("./OUTS/TOP/TEST/5/predict_huge") ;
				lossfiles.push_back ("./OUTS/TOP/TEST/6/predict_huge") ;
				lossfiles.push_back ("./OUTS/TOP/TEST/7/predict_huge") ;
			}
			names vectfiles ;
			/* Create the vector list: */ {
				vectfiles.push_back ("./OUTS/TOP/TEST/0/vector") ;
				vectfiles.push_back ("./OUTS/TOP/TEST/1/vector") ;
				vectfiles.push_back ("./OUTS/TOP/TEST/2/vector") ;
				vectfiles.push_back ("./OUTS/TOP/TEST/3/vector") ;
				vectfiles.push_back ("./OUTS/TOP/TEST/4/vector") ;
				vectfiles.push_back ("./OUTS/TOP/TEST/5/vector") ;
				vectfiles.push_back ("./OUTS/TOP/TEST/6/vector") ;
				vectfiles.push_back ("./OUTS/TOP/TEST/7/vector") ;
			}
			names nsubfiles ;
			/* Get list for nsub */ {
				nsubfiles.push_back("./OUTS/TOP/TEST/0/nsub");
				nsubfiles.push_back("./OUTS/TOP/TEST/1/nsub");
				nsubfiles.push_back("./OUTS/TOP/TEST/2/nsub");
				nsubfiles.push_back("./OUTS/TOP/TEST/3/nsub");
				nsubfiles.push_back("./OUTS/TOP/TEST/4/nsub");
				nsubfiles.push_back("./OUTS/TOP/TEST/5/nsub");
				nsubfiles.push_back("./OUTS/TOP/TEST/6/nsub");
				nsubfiles.push_back("./OUTS/TOP/TEST/7/nsub");
			}
			for(size_t index=0;index<8;index++){
				READER_Loss
					READER_LOSS		(
						lossfiles[index]
					)
				; //
				READER_Vector
					READER_VECTOR	(
						vectfiles[index]
					)
				; //
				READER_NSub
					READER_NSUB		(
						nsubfiles[index]
					)
				; //
				for(
					size_t i = 0 ;
					i < READER_LOSS() ;
					i++
				) {
					loss	= READER_LOSS(i)		;
					mass	= READER_VECTOR(i).m()	;
					nsub[0]	= READER_NSUB(i)[0]		;
					nsub[1]	= READER_NSUB(i)[1]		;
					nsub[2]	= READER_NSUB(i)[2]		;
					nsub[3]	= READER_NSUB(i)[3]		;
					nsub[4]	= READER_NSUB(i)[4]		;
					signalTree.Fill();
				}
			}
		}
		signalTree.Write();
		background.Write();
	}
    //
}
////////////////////////////////////////////////////////////////
namespace STEP6_CNN_RESPONSE_SUPERVISED_CLASSIFICATION {
    using namespace MISC ;
    //
    inline void
    Plotter () {
        using TYPE_RESPONSE =
            Tensors::Array::ND_ARRAY
                <2,TYPE_DATA>
        ;
        std::vector
            <ROC_ELEMENT>
                elements
        ;
        MyHistN <4,true>
            Hist (
                "CNNResponse",
                50,-0.01,1.01
            )
        ; //
        for(size_t index=0;index<8;index++){
            char tmp[512] ;
            /* TEST */ {
                /* QCD */ {
                    sprintf (
                        tmp,
                        "./OUTS/QCD/TEST/%ld/CNNPredict",
                        index
                    ) ; //
                    std::string filename (tmp) ;
                    CPPFileIO::FullFileReader
                        <TYPE_RESPONSE>
                            reader (filename)
                    ;
                    for(size_t i=0;i<reader();i++){
                        elements.push_back(
                            ROC_ELEMENT(reader(i)[0],false)
                        ) ; //
                        Hist.Fill<0>(reader(i)[0]);
                    }
                }
                /* TOP */ {
                    sprintf (
                        tmp,
                        "./OUTS/TOP/TEST/%ld/CNNPredict",
                        index
                    ) ; //
                    std::string filename (tmp) ;
                    CPPFileIO::FullFileReader
                        <TYPE_RESPONSE>
                            reader (filename)
                    ;
                    for(size_t i=0;i<reader();i++){
                        elements.push_back(
                            ROC_ELEMENT(reader(i)[0],true)
                        ) ; //
                        Hist.Fill<1>(reader(i)[0]);
                    }
                }
            }
            /* TRAIN */ {
                /* QCD */ {
                    sprintf (
                        tmp,
                        "./OUTS/QCD/TRAIN/%ld/CNNPredict",
                        index
                    ) ; //
                    std::string filename (tmp) ;
                    CPPFileIO::FullFileReader
                        <TYPE_RESPONSE>
                            reader (filename)
                    ;
                    for(size_t i=0;i<reader();i++){
                        Hist.Fill<2>(reader(i)[0]);
                    }
                }
                /* TOP */ {
                    sprintf (
                        tmp,
                        "./OUTS/TOP/TRAIN/%ld/CNNPredict",
                        index
                    ) ; //
                    std::string filename (tmp) ;
                    CPPFileIO::FullFileReader
                        <TYPE_RESPONSE>
                            reader (filename)
                    ;
                    for(size_t i=0;i<reader();i++){
                        Hist.Fill<3>(reader(i)[0]);
                    }
                }
            }
        }
        mkdir ("./OUTS/ROC/",0755) ;
        WriteROC (
            elements,
            "./OUTS/ROC/QCD_TOP.txt"
        ) ; //
    }
    //
    inline void
    PlotterDCGAN () {
        using TYPE_RESPONSE =
            Tensors::Array::ND_ARRAY
                <2,TYPE_DATA>
        ;
        std::vector
            <ROC_ELEMENT>
                elements
        ;
        MyHistN <4,true>
            Hist (
                "DCGANResponse",
                50,-0.01,1.01
            )
        ; //
        for(size_t index=0;index<8;index++){
            char tmp[512] ;
            /* TEST */ {
                /* QCD */ {
                    sprintf (
                        tmp,
                        "./OUTS/QCD/TEST/%ld/DCGAN_RESPONSE",
                        index
                    ) ; //
                    std::string filename (tmp) ;
                    CPPFileIO::FullFileReader
                        <TYPE_RESPONSE>
                            reader (filename)
                    ;
                    for(size_t i=0;i<reader();i++){
                        elements.push_back (
                            ROC_ELEMENT(reader(i)[0],false)
                        ) ; //
                        Hist.Fill<0>(reader(i)[0]);
                    }
                }
                /* TOP */ {
                    sprintf (
                        tmp,
                        "./OUTS/TOP/TEST/%ld/DCGAN_RESPONSE",
                        index
                    ) ; //
                    std::string filename (tmp) ;
                    CPPFileIO::FullFileReader
                        <TYPE_RESPONSE>
                            reader (filename)
                    ;
                    for(size_t i=0;i<reader();i++){
                        elements.push_back (
                            ROC_ELEMENT(reader(i)[0],true)
                        ) ; //
                        Hist.Fill<1>(reader(i)[0]);
                    }
                }
            }
            /* TRAIN */ if(false) {
                /* QCD */ {
                    sprintf (
                        tmp,
                        "./OUTS/QCD/TRAIN/%ld/DCGAN_RESPONSE",
                        index
                    ) ; //
                    std::string filename (tmp) ;
                    CPPFileIO::FullFileReader
                        <TYPE_RESPONSE>
                            reader (filename)
                    ;
                    for(size_t i=0;i<reader();i++){
                        Hist.Fill<2>(reader(i)[0]);
                    }
                }
                /* TOP */ {
                    sprintf (
                        tmp,
                        "./OUTS/TOP/TRAIN/%ld/DCGAN_RESPONSE",
                        index
                    ) ; //
                    std::string filename (tmp) ;
                    CPPFileIO::FullFileReader
                        <TYPE_RESPONSE>
                            reader (filename)
                    ;
                    for(size_t i=0;i<reader();i++){
                        Hist.Fill<3>(reader(i)[0]);
                    }
                }
            }
        }
        mkdir ("./OUTS/ROC/",0755) ;
        WriteROC (
            elements,
            "./OUTS/ROC/QCD_TOP_DCGAN.txt"
        ) ; //
    }
    //
    inline void
    PlotterQCDWBS () {
        using TYPE_RESPONSE =
            Tensors::Array::ND_ARRAY
                <2,TYPE_DATA>
        ;
        std::vector
            <ROC_ELEMENT>
                elements
        ;
        MyHistN <4,true>
            Hist (
                "CNNResponseQCDWBS",
                50,-0.01,1.01
            )
        ; //
        for(size_t index=0;index<8;index++){
            char tmp[512] ;
            /* TEST */ {
                /* QCD */ {
                    sprintf (
                        tmp,
                        "./OUTS/QCD/TEST/%ld/CNNPredictQCDWBS",
                        index
                    ) ; //
                    std::string filename (tmp) ;
                    CPPFileIO::FullFileReader
                        <TYPE_RESPONSE>
                            reader (filename)
                    ;
                    for(size_t i=0;i<reader();i++){
                        elements.push_back(
                            ROC_ELEMENT(reader(i)[0],false)
                        ) ; //
                        Hist.Fill<0>(reader(i)[0]);
                    }
                }
                /* TOP */ {
                    sprintf (
                        tmp,
                        "./OUTS/WBS/TEST/%ld/CNNPredictQCDWBS",
                        index
                    ) ; //
                    std::string filename (tmp) ;
                    CPPFileIO::FullFileReader
                        <TYPE_RESPONSE>
                            reader (filename)
                    ;
                    for(size_t i=0;i<reader();i++){
                        elements.push_back(
                            ROC_ELEMENT(reader(i)[0],true)
                        ) ; //
                        Hist.Fill<1>(reader(i)[0]);
                    }
                }
            }
            /* TRAIN */ {
                /* QCD */ {
                    sprintf (
                        tmp,
                        "./OUTS/QCD/TRAIN/%ld/CNNPredictQCDWBS",
                        index
                    ) ; //
                    std::string filename (tmp) ;
                    CPPFileIO::FullFileReader
                        <TYPE_RESPONSE>
                            reader (filename)
                    ;
                    for(size_t i=0;i<reader();i++){
                        Hist.Fill<2>(reader(i)[0]);
                    }
                }
                /* TOP */ {
                    sprintf (
                        tmp,
                        "./OUTS/WBS/TRAIN/%ld/CNNPredictQCDWBS",
                        index
                    ) ; //
                    std::string filename (tmp) ;
                    CPPFileIO::FullFileReader
                        <TYPE_RESPONSE>
                            reader (filename)
                    ;
                    for(size_t i=0;i<reader();i++){
                        Hist.Fill<3>(reader(i)[0]);
                    }
                }
            }
        }
        mkdir ("./OUTS/ROC/",0755) ;
        WriteROC (
            elements,
            "./OUTS/ROC/QCD_WBS.txt"
        ) ; //
    }
    //
    inline void
    MakeTMVARoot () {
        /* Create the directory structure: */ {
            mkdir("./OUTS/",0755);
            mkdir("./OUTS/TMP/",0755);
            mkdir("./OUTS/TMP/tmva/",0755);
        }
        TFile
            outfile (
                "./OUTS/TMP/tmva/tmva_class_example.root",
                "RECREATE"
            )
        ; //
        TTree signalTree ("TreeS","TreeS") ;
        TTree background ("TreeB","TreeB") ;
        float mass, cnnres, random ;
        signalTree.Branch ( "mass"   , & mass   ) ;
        signalTree.Branch ( "cnnres" , & cnnres ) ;
        signalTree.Branch ( "random" , & random ) ;
        background.Branch ( "mass"   , & mass   ) ;
        background.Branch ( "cnnres" , & cnnres ) ;
        background.Branch ( "random" , & random ) ;
        using TYPE_RESPONSE =
            Tensors::Array::ND_ARRAY
                <2,TYPE_DATA>
        ; //
        /* TEST: */ {
            /* QCD: */ {
                std::vector <std::string>
                    ListOfNamesCNN ,
                    ListOfNamesVectors
                ; /* Prepare the list: */ {
                    ListOfNamesCNN.push_back ("./OUTS/QCD/TEST/0/CNNPredict") ;
                    ListOfNamesCNN.push_back ("./OUTS/QCD/TEST/1/CNNPredict") ;
                    ListOfNamesCNN.push_back ("./OUTS/QCD/TEST/2/CNNPredict") ;
                    ListOfNamesCNN.push_back ("./OUTS/QCD/TEST/3/CNNPredict") ;
                    ListOfNamesCNN.push_back ("./OUTS/QCD/TEST/4/CNNPredict") ;
                    ListOfNamesCNN.push_back ("./OUTS/QCD/TEST/5/CNNPredict") ;
                    ListOfNamesCNN.push_back ("./OUTS/QCD/TEST/6/CNNPredict") ;
                    ListOfNamesCNN.push_back ("./OUTS/QCD/TEST/7/CNNPredict") ;
                    ListOfNamesVectors.push_back ("./OUTS/QCD/TEST/0/vector") ;
                    ListOfNamesVectors.push_back ("./OUTS/QCD/TEST/1/vector") ;
                    ListOfNamesVectors.push_back ("./OUTS/QCD/TEST/2/vector") ;
                    ListOfNamesVectors.push_back ("./OUTS/QCD/TEST/3/vector") ;
                    ListOfNamesVectors.push_back ("./OUTS/QCD/TEST/4/vector") ;
                    ListOfNamesVectors.push_back ("./OUTS/QCD/TEST/5/vector") ;
                    ListOfNamesVectors.push_back ("./OUTS/QCD/TEST/6/vector") ;
                    ListOfNamesVectors.push_back ("./OUTS/QCD/TEST/7/vector") ;
                }
                CPPFileIO::myrandgen <pcg64>
                    randgen (1,0.1,0.2)
                ; //
                for (size_t index=0;index<8;index++) {
                    CPPFileIO::FullFileReader
                        <TYPE_RESPONSE>
                            reader_res (
                                ListOfNamesCNN[index]
                            )
                    ; //
                    CPPFileIO::FullFileReader
                        <outvector4>
                            reader_vector (
                                ListOfNamesVectors[index]
                            )
                    ; //
                    size_t Limit =
                        CPPFileIO::mymin (
                            reader_res    () ,
                            reader_vector ()
                        )
                    ; //
                    for (size_t i=0;i<Limit;i++) {
                        mass   = reader_vector(i).m() ;
                        cnnres = reader_res(i)[1]     ;
                        random = randgen[0]           ;
                        background.Fill ()            ;
                    }
                }
            }
            /* TOP: */ {
                std::vector <std::string>
                    ListOfNamesCNN ,
                    ListOfNamesVectors
                ; /* Prepare the list: */ {
                    ListOfNamesCNN.push_back ("./OUTS/TOP/TEST/0/CNNPredict") ;
                    ListOfNamesCNN.push_back ("./OUTS/TOP/TEST/1/CNNPredict") ;
                    ListOfNamesCNN.push_back ("./OUTS/TOP/TEST/2/CNNPredict") ;
                    ListOfNamesCNN.push_back ("./OUTS/TOP/TEST/3/CNNPredict") ;
                    ListOfNamesCNN.push_back ("./OUTS/TOP/TEST/4/CNNPredict") ;
                    ListOfNamesCNN.push_back ("./OUTS/TOP/TEST/5/CNNPredict") ;
                    ListOfNamesCNN.push_back ("./OUTS/TOP/TEST/6/CNNPredict") ;
                    ListOfNamesCNN.push_back ("./OUTS/TOP/TEST/7/CNNPredict") ;
                    ListOfNamesVectors.push_back ("./OUTS/TOP/TEST/0/vector") ;
                    ListOfNamesVectors.push_back ("./OUTS/TOP/TEST/1/vector") ;
                    ListOfNamesVectors.push_back ("./OUTS/TOP/TEST/2/vector") ;
                    ListOfNamesVectors.push_back ("./OUTS/TOP/TEST/3/vector") ;
                    ListOfNamesVectors.push_back ("./OUTS/TOP/TEST/4/vector") ;
                    ListOfNamesVectors.push_back ("./OUTS/TOP/TEST/5/vector") ;
                    ListOfNamesVectors.push_back ("./OUTS/TOP/TEST/6/vector") ;
                    ListOfNamesVectors.push_back ("./OUTS/TOP/TEST/7/vector") ;
                }
                CPPFileIO::myrandgen <pcg64>
                    randgen (1,0.8,0.9)
                ; //
                for (size_t index=0;index<8;index++) {
                    CPPFileIO::FullFileReader
                        <TYPE_RESPONSE>
                            reader_res (
                                ListOfNamesCNN[index]
                            )
                    ; //
                    CPPFileIO::FullFileReader
                        <outvector4>
                            reader_vector (
                                ListOfNamesVectors[index]
                            )
                    ; //
                    size_t Limit =
                        CPPFileIO::mymin (
                            reader_res    () ,
                            reader_vector ()
                        )
                    ; //
                    for (size_t i=0;i<Limit;i++) {
                        mass   = reader_vector(i).m() ;
                        cnnres = reader_res(i)[1]     ;
                        random = randgen[0]           ;
                        signalTree.Fill ()            ;
                    }
                }
            }
        }
        signalTree.Write () ;
        background.Write () ;
    }
    //
}
////////////////////////////////////////////////////////////////
namespace STEP7_HTT_Compare {
    using namespace MISC ;
    inline void
    WriteHTT () {
        ROC_ELEMENTs Results ;
        /* For QCD: */ {
            std::vector
                <std::string>
                    Names
            ; /* Fill the names: */ {
                Names.push_back("./OUTS/QCD/TRAIN/0/toptag");
                Names.push_back("./OUTS/QCD/TRAIN/1/toptag");
                Names.push_back("./OUTS/QCD/TRAIN/2/toptag");
                Names.push_back("./OUTS/QCD/TRAIN/3/toptag");
                Names.push_back("./OUTS/QCD/TRAIN/4/toptag");
                Names.push_back("./OUTS/QCD/TRAIN/5/toptag");
                Names.push_back("./OUTS/QCD/TRAIN/6/toptag");
                Names.push_back("./OUTS/QCD/TRAIN/7/toptag");
            }
            for(size_t i=0;i<Names.size();i++){
                CPPFileIO::FullFileReader
                    < bool > Reader (Names[i])
                ; //
                for(size_t j=0;j<Reader();j++){
                    ROC_ELEMENT tmp ;
                    tmp.signal = false ;
                    if(Reader(j)){tmp.value=1.0;}
                    else{tmp.value=0.0;}
                    Results.push_back(tmp);
                }
            }
        }
        /* For TOP: */ {
            std::vector
                <std::string>
                    Names
            ; /* Fill the names: */ {
                Names.push_back("./OUTS/TOP/TRAIN/0/toptag");
                Names.push_back("./OUTS/TOP/TRAIN/1/toptag");
                Names.push_back("./OUTS/TOP/TRAIN/2/toptag");
                Names.push_back("./OUTS/TOP/TRAIN/3/toptag");
                Names.push_back("./OUTS/TOP/TRAIN/4/toptag");
                Names.push_back("./OUTS/TOP/TRAIN/5/toptag");
                Names.push_back("./OUTS/TOP/TRAIN/6/toptag");
                Names.push_back("./OUTS/TOP/TRAIN/7/toptag");
            }
            for(size_t i=0;i<Names.size();i++){
                CPPFileIO::FullFileReader
                    < bool > Reader (Names[i])
                ; //
                for(size_t j=0;j<Reader();j++){
                    ROC_ELEMENT tmp ;
                    tmp.signal = true ;
                    if(Reader(j)){tmp.value=1.0;}
                    else{tmp.value=0.0;}
                    Results.push_back(tmp);
                }
            }
        }
        WriteHTTROC (
            Results ,
            "./OUTS/ROC/HTT.txt"
        ) ; //
    }
}
////////////////////////////////////////////////////////////////
namespace STEP8_CNN_AE_RESPONSE_PLOT_AND_TMVAROOT {

    using namespace
        MISC
    ; //

    class CNN_LOSS_ROC {
    public:

		using ROC_OUT_DATA =
			Tensors::Array::ND_ARRAY
				<2,TYPE_DATA>
		; //



        inline void
        StoreTOP () {
            ROC_OUT_DATA Results ;
            Results[0] = mass ;
            Results[1] = loss ;
            TOP_Writer.push_back(Results);
            Hists.Fill<1>(loss);
            signalTree->Fill();
        }

        inline void
        StoreQCD () {
            ROC_OUT_DATA Results ;
            Results[0] = mass ;
            Results[1] = loss ;
            QCD_Writer.push_back(Results);
            Hists.Fill<0>(loss);
            background->Fill();
        }

		inline void
		PLOT_QCD () {

			names	list,	list2,	list3	;
			QCD_TEST_NAME_LIST		(list)	;
			QCD_TEST_VECTOR_LIST	(list2)	;
			QCD_TEST_NSUB_LIST		(list3)	;

			for (size_t j=0;j<list.size();j++) {

				READER_Loss		Reader	( list[j]	) ;
				READER_Vector	Reader2	( list2[j]	) ;
				READER_NSub		Reader3	( list3[j]	) ;

				size_t const min =
					CPPFileIO::mymin (
						Reader3()		,
						CPPFileIO::mymin (
							Reader()	,
							Reader2()
						)
					)
				; //

				for (size_t i=0;i<min;i++) {
					loss	= Reader(i)			;
					mass	= Reader2(i).m()	;
					nsub[0]	= Reader3(i)[0]		;
					nsub[1]	= Reader3(i)[1]		;
					nsub[2]	= Reader3(i)[2]		;
					nsub[3]	= Reader3(i)[3]		;
					nsub[4]	= Reader3(i)[4]		;
					StoreQCD	()				;
				}

			}

		}

		inline void
		PLOT_TOP () {

			names	list,	list2,	list3	;
			TOP_TEST_NAME_LIST		(list)	;
			TOP_TEST_VECTOR_LIST	(list2)	;
			TOP_TEST_NSUB_LIST		(list3)	;

			for (size_t j=0;j<list.size();j++) {

				READER_Loss		Reader	( list[j]	) ;
				READER_Vector	Reader2	( list2[j]	) ;
				READER_NSub		Reader3	( list3[j]	) ;

				size_t const min =
					CPPFileIO::mymin (
						Reader3()		,
						CPPFileIO::mymin (
							Reader()	,
							Reader2()
						)
					)
				; //

				for (size_t i=0;i<min;i++) {
					loss	= Reader(i)			;
					mass	= Reader2(i).m()	;
					nsub[0]	= Reader3(i)[0]		;
					nsub[1]	= Reader3(i)[1]		;
					nsub[2]	= Reader3(i)[2]		;
					nsub[3]	= Reader3(i)[3]		;
					nsub[4]	= Reader3(i)[4]		;
					StoreTOP	()				;
				}

			}
		}

        inline void
        PLOT_QCD_TRAIN () {

            names list , list2 ;
            QCD_TRAIN_NAME_LIST		(list)	;
            QCD_TRAIN_VECTOR_LIST	(list2)	;

            for(size_t j=0;j<list.size();j++){

                CPPFileIO::FullFileReader
                    <TYPE_DATA>
                    Reader (list[j])
                ; //

                CPPFileIO::FullFileReader
                    <outvector4>
                    Reader2 (list2[j])
                ; //

                size_t const min =
                    CPPFileIO::mymin (
                        Reader()	,
                        Reader2()
                    )
                ; //

                for(size_t i=0;i<min;i++) {
                    loss = Reader(i) ;
                    mass = Reader2(i).m() ;
                    Hists.Fill<2>(loss);
                }

            }

        }

        inline void
        PLOT_TOP_TRAIN () {

            names list , list2 ;
            TOP_TRAIN_NAME_LIST		(list)	;
            TOP_TRAIN_VECTOR_LIST	(list2)	;

            for(size_t j=0;j<list.size();j++){

                CPPFileIO::FullFileReader
                    <TYPE_DATA>
                    Reader (list[j])
                ; //

                CPPFileIO::FullFileReader
                    <outvector4>
                    Reader2 (list2[j])
                ; //

                size_t const min =
                    CPPFileIO::mymin (
                        Reader()	,
                        Reader2()
                    )
                ; //

                for(size_t i=0;i<min;i++) {
                    loss = Reader(i) ;
                    mass = Reader2(i).m() ;
                    Hists.Fill<3>(loss);
                }

            }

        }

        CNN_LOSS_ROC () :
        Hists ("Losses_CNN_AE",100,-0.01,0.3) ,
        QCD_Writer ("./OUTS/ROC/QCD_CNN_DATA") ,
        TOP_Writer ("./OUTS/ROC/TOP_CNN_DATA") {

            /* Prepare the directories: */ {
                mkdir ( "./OUTS/"			, 0755 ) ;
                mkdir ( "./OUTS/TMP/"		, 0755 ) ;
                mkdir ( "./OUTS/TMP/tmva/"	, 0755 ) ;
            }

            /* PREPARE ROOT FILE: */ {
                Outfile =
                    new TFile (
                        "./OUTS/TMP/CNN_LOSS_NORMAL.root",
                        "RECREATE"
                    )
                ;
            }

            /* PREPARE BACKGROUND TREE: */ {
                background =
                    new TTree (
                        "TreeB",
                        "TreeB"
                    )
                ; //
				background->Branch ( "mass"		, & mass	) ;
				background->Branch ( "loss"		, & loss	) ;
				background->Branch ( "nsub0"	, & nsub[0]	) ;
				background->Branch ( "nsub1"	, & nsub[2]	) ;
				background->Branch ( "nsub2"	, & nsub[3]	) ;
				background->Branch ( "nsub3"	, & nsub[3]	) ;
				background->Branch ( "nsub4"	, & nsub[4]	) ;
            }

            /* PREPARE SIGNAL TREE: */ {
                signalTree =
                    new TTree (
                        "TreeS",
                        "TreeS"
                    )
                ; //
				signalTree->Branch ( "mass"		, & mass	) ;
				signalTree->Branch ( "loss"		, & loss	) ;
				signalTree->Branch ( "nsub0"	, & nsub[0]	) ;
				signalTree->Branch ( "nsub1"	, & nsub[2]	) ;
				signalTree->Branch ( "nsub2"	, & nsub[3]	) ;
				signalTree->Branch ( "nsub3"	, & nsub[3]	) ;
				signalTree->Branch ( "nsub4"	, & nsub[4]	) ;
            }

            PLOT_QCD();
            PLOT_TOP();
            //PLOT_QCD_TRAIN();
            //PLOT_TOP_TRAIN();

        }

        ~CNN_LOSS_ROC(){
            signalTree->Write()	;
            background->Write()	;
            delete signalTree	;
            delete background	;
            delete Outfile		;
        }

        //////////////////////
        // NAME LIST BEGIN: //
        //////////////////////

		static inline void
		QCD_TEST_NSUB_LIST (
			names & in
		) {
			in.clear();
			in.push_back("./OUTS/QCD/TEST/0/nsub");
			in.push_back("./OUTS/QCD/TEST/1/nsub");
			in.push_back("./OUTS/QCD/TEST/2/nsub");
			in.push_back("./OUTS/QCD/TEST/3/nsub");
			in.push_back("./OUTS/QCD/TEST/4/nsub");
			in.push_back("./OUTS/QCD/TEST/5/nsub");
			in.push_back("./OUTS/QCD/TEST/6/nsub");
			in.push_back("./OUTS/QCD/TEST/7/nsub");
		}

		static inline void
		QCD_TRAIN_NSUB_LIST (
			names & in
		) {
			in.clear();
			in.push_back("./OUTS/QCD/TRAIN/0/nsub");
			in.push_back("./OUTS/QCD/TRAIN/1/nsub");
			in.push_back("./OUTS/QCD/TRAIN/2/nsub");
			in.push_back("./OUTS/QCD/TRAIN/3/nsub");
			in.push_back("./OUTS/QCD/TRAIN/4/nsub");
			in.push_back("./OUTS/QCD/TRAIN/5/nsub");
			in.push_back("./OUTS/QCD/TRAIN/6/nsub");
			in.push_back("./OUTS/QCD/TRAIN/7/nsub");
		}

		static inline void
		TOP_TRAIN_NSUB_LIST (
			names & in
		) {
			in.clear();
			in.push_back("./OUTS/TOP/TRAIN/0/nsub");
			in.push_back("./OUTS/TOP/TRAIN/1/nsub");
			in.push_back("./OUTS/TOP/TRAIN/2/nsub");
			in.push_back("./OUTS/TOP/TRAIN/3/nsub");
			in.push_back("./OUTS/TOP/TRAIN/4/nsub");
			in.push_back("./OUTS/TOP/TRAIN/5/nsub");
			in.push_back("./OUTS/TOP/TRAIN/6/nsub");
			in.push_back("./OUTS/TOP/TRAIN/7/nsub");
		}

		static inline void
		TOP_TEST_NSUB_LIST (
			names & in
		) {
			in.clear();
			in.push_back("./OUTS/TOP/TEST/0/nsub");
			in.push_back("./OUTS/TOP/TEST/1/nsub");
			in.push_back("./OUTS/TOP/TEST/2/nsub");
			in.push_back("./OUTS/TOP/TEST/3/nsub");
			in.push_back("./OUTS/TOP/TEST/4/nsub");
			in.push_back("./OUTS/TOP/TEST/5/nsub");
			in.push_back("./OUTS/TOP/TEST/6/nsub");
			in.push_back("./OUTS/TOP/TEST/7/nsub");
		}

        static inline void
        QCD_TRAIN_NAME_LIST (
            names & in
        ) {
            in.clear();
            in.push_back("./OUTS/QCD/TRAIN/0/loss_cnn");
            in.push_back("./OUTS/QCD/TRAIN/1/loss_cnn");
            in.push_back("./OUTS/QCD/TRAIN/2/loss_cnn");
            in.push_back("./OUTS/QCD/TRAIN/3/loss_cnn");
            in.push_back("./OUTS/QCD/TRAIN/4/loss_cnn");
            in.push_back("./OUTS/QCD/TRAIN/5/loss_cnn");
            in.push_back("./OUTS/QCD/TRAIN/6/loss_cnn");
            in.push_back("./OUTS/QCD/TRAIN/7/loss_cnn");
        }

        static inline void
        QCD_TRAIN_VECTOR_LIST (
            names & in
        ) {
            in.clear();
            in.push_back("./OUTS/QCD/TRAIN/0/vector");
            in.push_back("./OUTS/QCD/TRAIN/1/vector");
            in.push_back("./OUTS/QCD/TRAIN/2/vector");
            in.push_back("./OUTS/QCD/TRAIN/3/vector");
            in.push_back("./OUTS/QCD/TRAIN/4/vector");
            in.push_back("./OUTS/QCD/TRAIN/5/vector");
            in.push_back("./OUTS/QCD/TRAIN/6/vector");
            in.push_back("./OUTS/QCD/TRAIN/7/vector");
        }

        static inline void
        TOP_TRAIN_NAME_LIST (
            names & in
        ) {
            in.clear();
            in.push_back("./OUTS/TOP/TRAIN/0/loss_cnn");
            in.push_back("./OUTS/TOP/TRAIN/1/loss_cnn");
            in.push_back("./OUTS/TOP/TRAIN/2/loss_cnn");
            in.push_back("./OUTS/TOP/TRAIN/3/loss_cnn");
            in.push_back("./OUTS/TOP/TRAIN/4/loss_cnn");
            in.push_back("./OUTS/TOP/TRAIN/5/loss_cnn");
            in.push_back("./OUTS/TOP/TRAIN/6/loss_cnn");
            in.push_back("./OUTS/TOP/TRAIN/7/loss_cnn");
        }

        static inline void
        TOP_TRAIN_VECTOR_LIST (
            names & in
        ) {
            in.clear();
            in.push_back("./OUTS/TOP/TRAIN/0/vector");
            in.push_back("./OUTS/TOP/TRAIN/1/vector");
            in.push_back("./OUTS/TOP/TRAIN/2/vector");
            in.push_back("./OUTS/TOP/TRAIN/3/vector");
            in.push_back("./OUTS/TOP/TRAIN/4/vector");
            in.push_back("./OUTS/TOP/TRAIN/5/vector");
            in.push_back("./OUTS/TOP/TRAIN/6/vector");
            in.push_back("./OUTS/TOP/TRAIN/7/vector");
        }

        static inline void
        QCD_TEST_NAME_LIST (
            names & in
        ) {
            in.clear();
            in.push_back("./OUTS/QCD/TEST/0/loss_cnn");
            in.push_back("./OUTS/QCD/TEST/1/loss_cnn");
            in.push_back("./OUTS/QCD/TEST/2/loss_cnn");
            in.push_back("./OUTS/QCD/TEST/3/loss_cnn");
            in.push_back("./OUTS/QCD/TEST/4/loss_cnn");
            in.push_back("./OUTS/QCD/TEST/5/loss_cnn");
            in.push_back("./OUTS/QCD/TEST/6/loss_cnn");
            in.push_back("./OUTS/QCD/TEST/7/loss_cnn");
        }

        static inline void
        QCD_TEST_VECTOR_LIST (
            names & in
        ) {
            in.clear();
            in.push_back("./OUTS/QCD/TEST/0/vector");
            in.push_back("./OUTS/QCD/TEST/1/vector");
            in.push_back("./OUTS/QCD/TEST/2/vector");
            in.push_back("./OUTS/QCD/TEST/3/vector");
            in.push_back("./OUTS/QCD/TEST/4/vector");
            in.push_back("./OUTS/QCD/TEST/5/vector");
            in.push_back("./OUTS/QCD/TEST/6/vector");
            in.push_back("./OUTS/QCD/TEST/7/vector");
        }

        static inline void
        TOP_TEST_VECTOR_LIST (
            names & in
        ) {
            in.clear();
            in.push_back("./OUTS/TOP/TEST/0/vector");
            in.push_back("./OUTS/TOP/TEST/1/vector");
            in.push_back("./OUTS/TOP/TEST/2/vector");
            in.push_back("./OUTS/TOP/TEST/3/vector");
            in.push_back("./OUTS/TOP/TEST/4/vector");
            in.push_back("./OUTS/TOP/TEST/5/vector");
            in.push_back("./OUTS/TOP/TEST/6/vector");
            in.push_back("./OUTS/TOP/TEST/7/vector");
        }

        static inline void
        TOP_TEST_NAME_LIST (
            names & in
        ) {
            in.clear();
            in.push_back("./OUTS/TOP/TEST/0/loss_cnn");
            in.push_back("./OUTS/TOP/TEST/1/loss_cnn");
            in.push_back("./OUTS/TOP/TEST/2/loss_cnn");
            in.push_back("./OUTS/TOP/TEST/3/loss_cnn");
            in.push_back("./OUTS/TOP/TEST/4/loss_cnn");
            in.push_back("./OUTS/TOP/TEST/5/loss_cnn");
            in.push_back("./OUTS/TOP/TEST/6/loss_cnn");
            in.push_back("./OUTS/TOP/TEST/7/loss_cnn");
        }

		////////////////////
        // NAME LIST END. //
        ////////////////////

        ///////////////////
        // WORKER BEGIN: //
        ///////////////////

		static inline void
		PLOT_QCD_MASS_LOSS_BINS () {

			MyHistN <6,false>
				CNN_BINS (
					"CNN_LOSS_BINS",
					100,-0.01,0.61
				)
			; //

			names	list	,	list2	;
			QCD_TRAIN_NAME_LIST		(list)	;
			QCD_TRAIN_VECTOR_LIST	(list2)	;

			size_t constexpr masses[7] =
				{0,100,200,300,400,500,600}
			;

			for(size_t j=0;j<list.size();j++){

				CPPFileIO::FullFileReader
					<TYPE_DATA>
					Reader (list[j])
				; //

				CPPFileIO::FullFileReader
					<outvector4>
					Reader2 (list2[j])
				; //

				size_t const min =
					CPPFileIO::mymin (
						Reader()	,
						Reader2()
					)
				; //

				for(size_t i=0;i<min;i++) {
					TYPE_DATA loss = Reader(i) ;
					TYPE_DATA mass = Reader2(i).m() ;
					if (false) {
						if      ((masses[0]<=mass)&&(mass<masses[1])) {CNN_BINS.Fill<0>(loss);}
						else if ((masses[1]<=mass)&&(mass<masses[2])) {CNN_BINS.Fill<1>(loss);}
						else if ((masses[2]<=mass)&&(mass<masses[3])) {CNN_BINS.Fill<2>(loss);}
						else if ((masses[3]<=mass)&&(mass<masses[4])) {CNN_BINS.Fill<3>(loss);}
						else if ((masses[4]<=mass)&&(mass<masses[5])) {CNN_BINS.Fill<4>(loss);}
						else if ((masses[5]<=mass)&&(mass<masses[6])) {CNN_BINS.Fill<5>(loss);}
						else if (masses[6]<=mass)                     {CNN_BINS.Fill<6>(loss);}
					} else if (true) {
						if      ((masses[0]<=mass)&&(mass<masses[1]))	{CNN_BINS.Fill<0>(loss);}
						else if ((masses[1]<=mass)&&(mass<masses[2]))	{CNN_BINS.Fill<1>(loss);}
						else if ((masses[2]<=mass)&&(mass<masses[3]))	{CNN_BINS.Fill<2>(loss);}
						else if ((masses[3]<=mass)&&(mass<masses[4]))	{CNN_BINS.Fill<3>(loss);}
						else if ((masses[4]<=mass)&&(mass<masses[5]))	{CNN_BINS.Fill<4>(loss);}
						else if ((masses[5]<=mass))						{CNN_BINS.Fill<5>(loss);}
					}
				}

			}

		}

        static inline void
        WORK () {
			CNN_LOSS_ROC tmp ;
			PLOT_QCD_MASS_LOSS_BINS () ;
		}

		/////////////////
		// WORKER END. //
		/////////////////

		/////////////////
		// DATA BEGIN: //
		/////////////////

		MyHistN <4,true>
            Hists
        ; //

        TFile *	Outfile		;
        TTree *	signalTree	;
        TTree *	background	;
        float	loss		;
        float	mass		;
		float	nsub[5]		;

        CPPFileIO::FileVector
        <ROC_OUT_DATA>
            QCD_Writer	,
            TOP_Writer
        ; //

		///////////////
		// DATA END. //
		///////////////

    } ;

}
////////////////////////////////////////////////////////////////
namespace STEP9_GENERATE_HIGH_PT_QCD {

    using namespace MISC ;

	inline void
	Generate_QCD (
		size_t		index					,
		std::string	MidName					,
		size_t		random_seed	= 123		,
		size_t		N_Events	= 100000
	) {
        CPPFileIO::SetCPUAffinity(index);
        std::string prefixname ;
        /* Make the directories: */ {
            std::string dirname = "./OUTS/" ;
            mkdir ( &(dirname[0]) , 0755 ) ;
            dirname = "./OUTS/QCD_HPT/" ;
            mkdir ( &(dirname[0]) , 0755 ) ;
            dirname = "./OUTS/QCD_HPT/" + MidName + "/" ;
            mkdir ( &(dirname[0]) , 0755 ) ;
            char tmp = '0' ;
            tmp = tmp + ( (char) index ) ;
            dirname = dirname + tmp + "/" ;
            mkdir ( &(dirname[0]) , 0755 ) ;
            prefixname = dirname ;
        }
        size_t count = 0 ;
        EventWriter
            writer (
                prefixname
            )
        ; //
        MyPythia
            pythia
        ; /* Configure pythia: */ {
			pythia.readString ( "Beams:eCM = 100000"			) ;
			pythia.readString ( "PhaseSpace:pTHatMin = 9500"	) ;
			pythia.readString ( "PartonLevel:MPI = off"			) ;
			pythia.readString ( "Random:setSeed = on"			) ;
            /* Set the random number Seed */ {
                char tmp[256] ;
                sprintf (
                    tmp                 ,
                    "Random:seed = %ld" ,
                    index + random_seed
                ) ; //
                pythia.readString
                    (tmp)
                ; //
            }
            pythia.readString ( "HardQCD:all = on" ) ;
            pythia.init () ;
        }
        while (
            count<N_Events
        ) if (
            pythia.next()
        ) {
            vector4s hadrons ;
            /* Prepare the list of hadrons: */ {
                for (
                    size_t i=0;
                    i<pythia.event.size();
                    i++
                ) if (
                    pythia.event[i]
                        .isFinal()
                ) {
                    int pid =
                        pythia
                        .event[i]
                        .id()
                    ; //
                    if (pid<0) {pid=-pid;}
                    if (
                        ( pid != 12 ) &&
                        ( pid != 14 ) &&
                        ( pid != 16 )
                    ) {
                        vector4 tmp (
                            pythia.event[i].px() ,
                            pythia.event[i].py() ,
                            pythia.event[i].pz() ,
                            pythia.event[i].e()
                        ) ;
                        auto tmpeta =
                            tmp.rapidity()
                        ; //
                        if (
                            ( -2.5     < tmpeta ) &&
                            ( tmpeta   < 2.5    ) &&
                            ( tmp.pt() > 0.5    )
                        ) {
                            hadrons.push_back
                                (tmp)
                            ; //
                        }
                    }
                }
            }
            fastjet::JetAlgorithm algo =
                fastjet::antikt_algorithm
            ; //
            fastjet::JetDefinition
                jet_def (
                    algo,
                    1.0
                )
            ; //
            fastjet::ClusterSequence
                clust_seq (
                    hadrons,
                    jet_def
                )
            ; //
            vector4s jets =
                clust_seq
                .inclusive_jets
                    (800.0)
            ; //
            if (
                (jets.size()>0) &&
                (jets[0].constituents().size()>2)
            ) {
                auto tmppt =
                    jets[0].pt ()
                ; //
                auto tmpet =
                    jets[0].rapidity ()
                ; //
                if (
                    (
                        ( 10000 < tmppt )
                    ) && (
                        ( -2.5 < tmpet ) &&
                        ( tmpet < 2.5 )
                    )
                ) {
                    writer(jets[0]);
                    count++ ;
                }
            }
        }
    }

    inline void
    Generate_Train () {
        CPPFileIO::ForkMe forker ;
        for(size_t i=0;i<8;i++)
        if(forker.InKid()){
            Generate_QCD (
                i       ,
                "TRAIN" ,
                123     ,
                300000
            ) ; //
        }
    }

	inline void
	Generate_Test () {
		CPPFileIO::ForkMe forker ;
		for(size_t i=0;i<8;i++)
		if(forker.InKid()){
			Generate_QCD (
				i      ,
				"TEST" ,
				456    ,
				50000
			) ; //
		}
	}

    inline void
    PlotImages () {

        /* TRAIN */ if(false) {

            /* NORMAL */ {
                Plot2D
                    tmp("QCD_TRAIN_IMAGES")
                ; /* Record the file names: */ {
                    tmp("./OUTS/QCD/TRAIN/0/image");
                    tmp("./OUTS/QCD/TRAIN/1/image");
                    tmp("./OUTS/QCD/TRAIN/2/image");
                    tmp("./OUTS/QCD/TRAIN/3/image");
                    tmp("./OUTS/QCD/TRAIN/4/image");
                    tmp("./OUTS/QCD/TRAIN/5/image");
                    tmp("./OUTS/QCD/TRAIN/6/image");
                    tmp("./OUTS/QCD/TRAIN/7/image");
                }
            }

            /* UNBOOSTED */ {
                Plot2D
                    tmp("QCD_TRAIN_IMAGES_UNBOOSTED")
                ; /* Record the file names: */ {
                    tmp("./OUTS/QCD/TRAIN/0/UnBoostedimage");
                    tmp("./OUTS/QCD/TRAIN/1/UnBoostedimage");
                    tmp("./OUTS/QCD/TRAIN/2/UnBoostedimage");
                    tmp("./OUTS/QCD/TRAIN/3/UnBoostedimage");
                    tmp("./OUTS/QCD/TRAIN/4/UnBoostedimage");
                    tmp("./OUTS/QCD/TRAIN/5/UnBoostedimage");
                    tmp("./OUTS/QCD/TRAIN/6/UnBoostedimage");
                    tmp("./OUTS/QCD/TRAIN/7/UnBoostedimage");
                }
            }

            /* NOGRAMSCHMIDT */ {
                Plot2D
                    tmp("QCD_TRAIN_IMAGES_NOGRAMSCHMIDT")
                ; /* Record the file names: */ {
                    tmp("./OUTS/QCD/TRAIN/0/NoGramSchmidtImageType");
                    tmp("./OUTS/QCD/TRAIN/1/NoGramSchmidtImageType");
                    tmp("./OUTS/QCD/TRAIN/2/NoGramSchmidtImageType");
                    tmp("./OUTS/QCD/TRAIN/3/NoGramSchmidtImageType");
                    tmp("./OUTS/QCD/TRAIN/4/NoGramSchmidtImageType");
                    tmp("./OUTS/QCD/TRAIN/5/NoGramSchmidtImageType");
                    tmp("./OUTS/QCD/TRAIN/6/NoGramSchmidtImageType");
                    tmp("./OUTS/QCD/TRAIN/7/NoGramSchmidtImageType");
                }
            }

        }

        /* TEST */ if(true) {

            /* NORMAL */ {
                Plot2D
                    tmp("QCD_TEST_IMAGES")
                ; /* Record the file names: */ {
                    tmp("./OUTS/QCD/TEST/0/image");
                    tmp("./OUTS/QCD/TEST/1/image");
                    tmp("./OUTS/QCD/TEST/2/image");
                    tmp("./OUTS/QCD/TEST/3/image");
                    tmp("./OUTS/QCD/TEST/4/image");
                    tmp("./OUTS/QCD/TEST/5/image");
                    tmp("./OUTS/QCD/TEST/6/image");
                    tmp("./OUTS/QCD/TEST/7/image");
                }
            }

            /* UNBOOSTED */ {
                Plot2D
                    tmp("QCD_TEST_IMAGES_UNBOOSTED")
                ; /* Record the file names: */ {
                    tmp("./OUTS/QCD/TEST/0/UnBoostedimage");
                    tmp("./OUTS/QCD/TEST/1/UnBoostedimage");
                    tmp("./OUTS/QCD/TEST/2/UnBoostedimage");
                    tmp("./OUTS/QCD/TEST/3/UnBoostedimage");
                    tmp("./OUTS/QCD/TEST/4/UnBoostedimage");
                    tmp("./OUTS/QCD/TEST/5/UnBoostedimage");
                    tmp("./OUTS/QCD/TEST/6/UnBoostedimage");
                    tmp("./OUTS/QCD/TEST/7/UnBoostedimage");
                }
            }

            /* NOGRAMSCHMIDT */ {
                Plot2D
                    tmp("QCD_TEST_IMAGES_NOGRAMSCHMIDT")
                ; /* Record the file names: */ {
                    tmp("./OUTS/QCD/TEST/0/NoGramSchmidtImageType");
                    tmp("./OUTS/QCD/TEST/1/NoGramSchmidtImageType");
                    tmp("./OUTS/QCD/TEST/2/NoGramSchmidtImageType");
                    tmp("./OUTS/QCD/TEST/3/NoGramSchmidtImageType");
                    tmp("./OUTS/QCD/TEST/4/NoGramSchmidtImageType");
                    tmp("./OUTS/QCD/TEST/5/NoGramSchmidtImageType");
                    tmp("./OUTS/QCD/TEST/6/NoGramSchmidtImageType");
                    tmp("./OUTS/QCD/TEST/7/NoGramSchmidtImageType");
                }
            }

			/* NOGRAMSCHMIDT_BAD */ {
				Plot2D
					tmp("QCD_TEST_IMAGES_NOGRAMSCHMIDT_BAD")
				; /* Record the file names: */ {
					tmp("./OUTS/QCD/TEST/0/NoGramSchmidtBADImageType");
					tmp("./OUTS/QCD/TEST/1/NoGramSchmidtBADImageType");
					tmp("./OUTS/QCD/TEST/2/NoGramSchmidtBADImageType");
					tmp("./OUTS/QCD/TEST/3/NoGramSchmidtBADImageType");
					tmp("./OUTS/QCD/TEST/4/NoGramSchmidtBADImageType");
					tmp("./OUTS/QCD/TEST/5/NoGramSchmidtBADImageType");
					tmp("./OUTS/QCD/TEST/6/NoGramSchmidtBADImageType");
					tmp("./OUTS/QCD/TEST/7/NoGramSchmidtBADImageType");
				}
			}

        }

    }

    inline void
    PlotImages_1 () {

        /* TRAIN */ if(true) {

            /* NORMAL */ {
                Plot2D_1
                    tmp("QCD_TRAIN_IMAGES_1")
                ; /* Record the file names: */ {
                    tmp("./OUTS/QCD/TRAIN/0/image");
                }
            }

            /* UNBOOSTED */ {
                Plot2D_1
                    tmp("QCD_TRAIN_IMAGES_UNBOOSTED_1")
                ; /* Record the file names: */ {
                    tmp("./OUTS/QCD/TRAIN/0/UnBoostedimage");
                }
            }

            /* NOGRAMSCHMIDT */ {
                Plot2D_1
                    tmp("QCD_TRAIN_IMAGES_NOGRAMSCHMIDT_1")
                ; /* Record the file names: */ {
                    tmp("./OUTS/QCD/TRAIN/0/NoGramSchmidtImageType");
                }
            }

        }

    }

    inline void
    PlotNsub () {
        //
        /* TRAIN */ if(true) {
            MyHistN <5,false>
                nsubcompare (
                    "TAU_QCD_TRAIN",100,
                    -0.01,100.01
                )
            ; //
            for(
                size_t index=0;
                index<8;
                index++
            ) {
                char tmp[256] ;
                sprintf (
                    tmp,
                    "./OUTS/QCD/TRAIN/%ld/nsub",
                    index
                ) ; //
                CPPFileIO::FullFileReader
                    <TYPE_NSub>
                        Reader (tmp)
                ; //
                for (
                    size_t i=0;
                    i<Reader();
                    i++
                ) {
                    //
                    nsubcompare.Fill<0>
                        (Reader(i)[0])
                    ; //
                    nsubcompare.Fill<1>
                        (Reader(i)[1])
                    ; //
                    nsubcompare.Fill<2>
                        (Reader(i)[2])
                    ; //
                    nsubcompare.Fill<3>
                        (Reader(i)[3])
                    ; //
                    nsubcompare.Fill<4>
                        (Reader(i)[4])
                    ; //
                    //
                }
            }
        }
        //
        /* TEST */ if(true) {
            MyHistN <5,false>
                nsubcompare (
                    "TAU_QCD_TEST",100,
                    -0.01,100.01
                )
            ; //
            for(
                size_t index=0;
                index<8;
                index++
            ) {
                char tmp[256] ;
                sprintf (
                    tmp,
                    "./OUTS/QCD/TEST/%ld/nsub",
                    index
                ) ; //
                CPPFileIO::FullFileReader
                    <TYPE_NSub>
                        Reader (tmp)
                ; //
                for (
                    size_t i=0;
                    i<Reader();
                    i++
                ) {
                    //
                    nsubcompare.Fill<0>
                        (Reader(i)[0])
                    ; //
                    nsubcompare.Fill<1>
                        (Reader(i)[1])
                    ; //
                    nsubcompare.Fill<2>
                        (Reader(i)[2])
                    ; //
                    nsubcompare.Fill<3>
                        (Reader(i)[3])
                    ; //
                    nsubcompare.Fill<4>
                        (Reader(i)[4])
                    ; //
                    //
                }
            }
        }
        //
    }

}
////////////////////////////////////////////////////////////////
// MASS IN BINS OF EPSILON
namespace STEP10_2D_MASS_ERROR {

	using namespace MISC ;

	class Touple {
	public:

		using TYPE_SELF =
			Touple
		;

		inline bool
		operator > (
			TYPE_SELF const &
				other
		) const {
			return
				loss	>
				other.loss
			;
		}

		inline bool
		operator < (
			TYPE_SELF const &
				other
		) const {
			return
				loss	<
				other.loss
			;
		}

		Touple(
			TYPE_DATA const _loss ,
			TYPE_DATA const _mass
		) {
			loss = _loss ;
			mass = _mass ;
		}

		~Touple(){}

		TYPE_DATA
			loss	,
			mass
		; //

	} ;

	// MASS IN BINS OF EPSILON
	class PLOT_LOSS_MASS {
	public:

		inline void
		fill_2 (
			TYPE_DATA	const & U	,
			outvector4	const & V
		) {
			TYPE_DATA M = V.m() ;
			Hist.Fill(M,U);
			TYPE_DATA
				Splits[8]
			; /* Allocate the splits: */ {
				if (false) {
					Splits[0] = 0.000 ;
					Splits[1] = 0.020 ;
					Splits[2] = 0.040 ;
					Splits[3] = 0.060 ;
					Splits[4] = 0.080 ;
					Splits[5] = 1.000 ;
					Splits[6] = 1.020 ;
				} else {
					//printf("DEBUG: Came here...\n");
					Splits[0] = 0.00 ;
					Splits[1] = 0.15 ;
					Splits[2] = 0.30 ;
					Splits[3] = 0.45 ;
					Splits[4] = 0.60 ;
					Splits[5] = 0.75 ;
					Splits[6] = 0.90 ;
					Splits[7] = 1.05 ;
				}

			}
			if      ((Splits[0]<=U)&&(U<Splits[1])) {LossMassBins.Fill<0>(M);stats[0]++;}
			else if ((Splits[1]<=U)&&(U<Splits[2])) {LossMassBins.Fill<1>(M);stats[1]++;}
			else if ((Splits[2]<=U)&&(U<Splits[3])) {LossMassBins.Fill<2>(M);stats[2]++;}
			else if ((Splits[3]<=U)&&(U<Splits[4])) {LossMassBins.Fill<3>(M);stats[3]++;}
			else if ((Splits[4]<=U)&&(U<Splits[5])) {LossMassBins.Fill<4>(M);stats[4]++;}
			else if ((Splits[5]<=U)&&(U<Splits[6])) {LossMassBins.Fill<5>(M);stats[5]++;}
			else if ((Splits[6]<=U)&&(U<Splits[7])) {LossMassBins.Fill<6>(M);stats[6]++;}
		}

		inline void
		fill (
			TYPE_DATA	const & U	,
			outvector4	const & V
		) {
			TYPE_DATA M = V.m() ;
			Touple tmp (U,M) ;
			list.push_back(tmp);
		}

//////////////////////////
#define _MACRO_BINS_1_	\
	TYPE_DATA const		\
		sizes[8] = {	\
			0.00	,	\
			0.05	,	\
			0.20	,	\
			0.40	,	\
			0.60	,	\
			0.80	,	\
			0.95	,	\
			1.00		\
		}				\
	;					\
	bool constexpr		\
		doall = true	\
	;					//
//////////////////////////

//////////////////////////
#define _MACRO_BINS_2_	\
	TYPE_DATA const		\
		sizes[8] = {	\
			0.00	,	\
			0.15	,	\
			0.30	,	\
			0.45	,	\
			0.60	,	\
			0.75	,	\
			0.90	,	\
			1.05		\
		}				\
	;					\
	bool constexpr		\
		doall = false	\
	;					//
//////////////////////////

		inline void
		fill_all () {

			_MACRO_BINS_1_

			std::sort (
				list.begin	() ,
				list.end	()
			) ; //

			size_t const
				limit =
				list.size()
			; //

			for(size_t i=0;i<limit;i++){

				Hist.Fill (
					list[i].mass ,
					list[i].loss
				) ;

				double const frac =
					static_cast<double>(i)		/
					static_cast<double>(limit)
				; //

				LossMassBins2.fixedy = true ;

				if        ((sizes[0]<=frac)&&(frac<sizes[1])) {
					LossMassBins2.Fill<0>(list[i].mass);
				} else if ((sizes[1]<=frac)&&(frac<sizes[2])) {
					LossMassBins2.Fill<1>(list[i].mass);
					if(doall){
						LossMassBins2.Fill<0>(list[i].mass);
					}
				} else if ((sizes[2]<=frac)&&(frac<sizes[3])) {
					LossMassBins2.Fill<2>(list[i].mass);
					if(doall){
						LossMassBins2.Fill<0>(list[i].mass);
						LossMassBins2.Fill<1>(list[i].mass);
					}
				} else if ((sizes[3]<=frac)&&(frac<sizes[4])) {
					LossMassBins2.Fill<3>(list[i].mass);
					if(doall){
						LossMassBins2.Fill<0>(list[i].mass);
						LossMassBins2.Fill<1>(list[i].mass);
						LossMassBins2.Fill<2>(list[i].mass);
					}
				} else if ((sizes[4]<=frac)&&(frac<sizes[5])) {
					LossMassBins2.Fill<4>(list[i].mass);
					if(doall){
						LossMassBins2.Fill<0>(list[i].mass);
						LossMassBins2.Fill<1>(list[i].mass);
						LossMassBins2.Fill<2>(list[i].mass);
						LossMassBins2.Fill<3>(list[i].mass);
					}
				} else if ((sizes[5]<=frac)&&(frac<sizes[6])) {
					LossMassBins2.Fill<5>(list[i].mass);
					if(doall){
						LossMassBins2.Fill<0>(list[i].mass);
						LossMassBins2.Fill<1>(list[i].mass);
						LossMassBins2.Fill<2>(list[i].mass);
						LossMassBins2.Fill<3>(list[i].mass);
						LossMassBins2.Fill<4>(list[i].mass);
					}
				} else if ((sizes[6]<=frac)&&(frac<sizes[7])) {
					LossMassBins2.Fill<6>(list[i].mass);
					if(doall){
						LossMassBins2.Fill<0>(list[i].mass);
						LossMassBins2.Fill<1>(list[i].mass);
						LossMassBins2.Fill<2>(list[i].mass);
						LossMassBins2.Fill<3>(list[i].mass);
						LossMassBins2.Fill<4>(list[i].mass);
						LossMassBins2.Fill<5>(list[i].mass);
					}
				}

			}

		}

//////////////////////////
#undef _MACRO_BINS_1_	//
#undef _MACRO_BINS_2_	//
//////////////////////////

		inline void
		fill (
			std::string const & U ,
			std::string const & V
		) {
			CPPFileIO::FullFileReader
				<TYPE_DATA>
					ReaderU (U)
			; //
			CPPFileIO::FullFileReader
				<outvector4>
					ReaderV (V)
			; //
			size_t limit =
				CPPFileIO::mymin(
					ReaderU()	,
					ReaderV()
				)
			; //
			for(size_t i=0;i<limit;i++){
				fill (
					ReaderU(i)	,
					ReaderV(i)
				) ;
			}
		}

		inline void
		fill (
			std::vector <std::string> U ,
			std::vector <std::string> V
		) {
			size_t limit =
				CPPFileIO::mymin(
					U.size()	,
					V.size()
				)
			; //
			for(size_t i=0;i<limit;i++)
			{ fill(U[i],V[i]); }
		}

		PLOT_LOSS_MASS (
			std::string _histname ,
			TYPE_DATA const _limit
		) :
		histname(_histname) ,
		Hist (
			&(histname[0])	,
			&(histname[0])	,
			100, -0.01, _limit ,
			100, -0.1, 0.8
		) ,
		LossMassBins (
			histname+"bins",
			50, -0.01, _limit
		) ,
		LossMassBins2 (
			histname+"bins2",
			50, -0.01, _limit
		) {
			for(size_t i=0;i<7;i++){
				stats[i] = 0 ;
			}
			LossMassBins.fixedy = true ;
			LossMassBins2.fixedy = true ;

		}

		~PLOT_LOSS_MASS(){

			/* Write out stats: */ {
				mkdir("./OUTS",0755);
				mkdir("./OUTS/STATS",0755);
				std::string dirname("./OUTS/STATS/") ;
				std::string statname =
					dirname + histname + "bins"
				; //
				FILE *f =
					fopen (&(statname[0]),"w")
				; /* Write out the file: */ {
					for(size_t i=0;i<7;i++){
						fprintf(f,"%ld\n",stats[i]);
					}
				}
				fclose(f);
			}


			std::string
				filename(
					"./OUTS/GRAPHS/"
				)
			; /* Prepare the file name: */ {
				filename =
					filename +
					histname + ".pdf"
				; //
			}

			fill_all () ;

			TCanvas C ;
			Hist.Draw("colz");
			C.SaveAs(&(filename[0]));
		}

		std::string histname ;
		TH2F Hist ;
		MyHistN <7,false> LossMassBins	;
		MyHistN <7,false> LossMassBins2	;
		size_t stats[7] ;
		std::vector<Touple>list;

		static inline void
		WORK_QCDHPT_TEST () {
			std::vector <
				std::string
			> U ; /* Get Loss name list: */ {
				U.push_back("./OUTS/QCD_HPT/TEST/0/predict");
				U.push_back("./OUTS/QCD_HPT/TEST/1/predict");
				U.push_back("./OUTS/QCD_HPT/TEST/2/predict");
				U.push_back("./OUTS/QCD_HPT/TEST/3/predict");
				U.push_back("./OUTS/QCD_HPT/TEST/4/predict");
				U.push_back("./OUTS/QCD_HPT/TEST/5/predict");
				U.push_back("./OUTS/QCD_HPT/TEST/6/predict");
				U.push_back("./OUTS/QCD_HPT/TEST/7/predict");
			}

			std::vector <
				std::string
			> V ; /* Get Vector name list: */ {
				V.push_back("./OUTS/QCD_HPT/TEST/0/vector");
				V.push_back("./OUTS/QCD_HPT/TEST/1/vector");
				V.push_back("./OUTS/QCD_HPT/TEST/2/vector");
				V.push_back("./OUTS/QCD_HPT/TEST/3/vector");
				V.push_back("./OUTS/QCD_HPT/TEST/4/vector");
				V.push_back("./OUTS/QCD_HPT/TEST/5/vector");
				V.push_back("./OUTS/QCD_HPT/TEST/6/vector");
				V.push_back("./OUTS/QCD_HPT/TEST/7/vector");
			}

			PLOT_LOSS_MASS
				tmp (
					"Mass_Loss_QCDHPT_TEST" ,
					8000.0
				)
			; //

			tmp.fill(U,V);

		}

		static inline void
		WORK_QCDHPT_TRAIN () {
			std::vector <
				std::string
			> U ; /* Get Loss name list: */ {
				U.push_back("./OUTS/QCD_HPT/TRAIN/0/predict");
				U.push_back("./OUTS/QCD_HPT/TRAIN/1/predict");
				U.push_back("./OUTS/QCD_HPT/TRAIN/2/predict");
				U.push_back("./OUTS/QCD_HPT/TRAIN/3/predict");
				U.push_back("./OUTS/QCD_HPT/TRAIN/4/predict");
				U.push_back("./OUTS/QCD_HPT/TRAIN/5/predict");
				U.push_back("./OUTS/QCD_HPT/TRAIN/6/predict");
				U.push_back("./OUTS/QCD_HPT/TRAIN/7/predict");
			}

			std::vector <
				std::string
			> V ; /* Get Vector name list: */ {
				V.push_back("./OUTS/QCD_HPT/TRAIN/0/vector");
				V.push_back("./OUTS/QCD_HPT/TRAIN/1/vector");
				V.push_back("./OUTS/QCD_HPT/TRAIN/2/vector");
				V.push_back("./OUTS/QCD_HPT/TRAIN/3/vector");
				V.push_back("./OUTS/QCD_HPT/TRAIN/4/vector");
				V.push_back("./OUTS/QCD_HPT/TRAIN/5/vector");
				V.push_back("./OUTS/QCD_HPT/TRAIN/6/vector");
				V.push_back("./OUTS/QCD_HPT/TRAIN/7/vector");
			}

			PLOT_LOSS_MASS
				tmp (
					"Mass_Loss_QCDHPT_TRAIN" ,
					8000.0
				)
			; //

			tmp.fill(U,V);

		}

		static inline void
		WORK_QCDHPT_FULL () {
			std::vector <
				std::string
			> U ; /* Get Loss name list: */ {
				U.push_back("./OUTS/QCD_HPT/TRAIN/0/predict");
				U.push_back("./OUTS/QCD_HPT/TRAIN/1/predict");
				U.push_back("./OUTS/QCD_HPT/TRAIN/2/predict");
				U.push_back("./OUTS/QCD_HPT/TRAIN/3/predict");
				U.push_back("./OUTS/QCD_HPT/TRAIN/4/predict");
				U.push_back("./OUTS/QCD_HPT/TRAIN/5/predict");
				U.push_back("./OUTS/QCD_HPT/TRAIN/6/predict");
				U.push_back("./OUTS/QCD_HPT/TRAIN/7/predict");
				U.push_back("./OUTS/QCD_HPT/TEST/0/predict");
				U.push_back("./OUTS/QCD_HPT/TEST/1/predict");
				U.push_back("./OUTS/QCD_HPT/TEST/2/predict");
				U.push_back("./OUTS/QCD_HPT/TEST/3/predict");
				U.push_back("./OUTS/QCD_HPT/TEST/4/predict");
				U.push_back("./OUTS/QCD_HPT/TEST/5/predict");
				U.push_back("./OUTS/QCD_HPT/TEST/6/predict");
				U.push_back("./OUTS/QCD_HPT/TEST/7/predict");
			}

			std::vector <
				std::string
			> V ; /* Get Vector name list: */ {
				V.push_back("./OUTS/QCD_HPT/TRAIN/0/vector");
				V.push_back("./OUTS/QCD_HPT/TRAIN/1/vector");
				V.push_back("./OUTS/QCD_HPT/TRAIN/2/vector");
				V.push_back("./OUTS/QCD_HPT/TRAIN/3/vector");
				V.push_back("./OUTS/QCD_HPT/TRAIN/4/vector");
				V.push_back("./OUTS/QCD_HPT/TRAIN/5/vector");
				V.push_back("./OUTS/QCD_HPT/TRAIN/6/vector");
				V.push_back("./OUTS/QCD_HPT/TRAIN/7/vector");
				V.push_back("./OUTS/QCD_HPT/TEST/0/vector");
				V.push_back("./OUTS/QCD_HPT/TEST/1/vector");
				V.push_back("./OUTS/QCD_HPT/TEST/2/vector");
				V.push_back("./OUTS/QCD_HPT/TEST/3/vector");
				V.push_back("./OUTS/QCD_HPT/TEST/4/vector");
				V.push_back("./OUTS/QCD_HPT/TEST/5/vector");
				V.push_back("./OUTS/QCD_HPT/TEST/6/vector");
				V.push_back("./OUTS/QCD_HPT/TEST/7/vector");
			}

			PLOT_LOSS_MASS
				tmp (
					"Mass_Loss_QCDHPT_FULL" ,
					8000.0
				)
			; //

			tmp.fill(U,V);

		}

		static inline void
		WORK_QCD_TEST () {
			std::vector <
				std::string
			> U ; /* Get Loss name list: */ {
				U.push_back("./OUTS/QCD/TEST/0/predict");
				U.push_back("./OUTS/QCD/TEST/1/predict");
				U.push_back("./OUTS/QCD/TEST/2/predict");
				U.push_back("./OUTS/QCD/TEST/3/predict");
				U.push_back("./OUTS/QCD/TEST/4/predict");
				U.push_back("./OUTS/QCD/TEST/5/predict");
				U.push_back("./OUTS/QCD/TEST/6/predict");
				U.push_back("./OUTS/QCD/TEST/7/predict");
			}

			std::vector <
				std::string
			> V ; /* Get Vector name list: */ {
				V.push_back("./OUTS/QCD/TEST/0/vector");
				V.push_back("./OUTS/QCD/TEST/1/vector");
				V.push_back("./OUTS/QCD/TEST/2/vector");
				V.push_back("./OUTS/QCD/TEST/3/vector");
				V.push_back("./OUTS/QCD/TEST/4/vector");
				V.push_back("./OUTS/QCD/TEST/5/vector");
				V.push_back("./OUTS/QCD/TEST/6/vector");
				V.push_back("./OUTS/QCD/TEST/7/vector");
			}

			PLOT_LOSS_MASS
				tmp (
					"Mass_Loss_QCD_TEST" ,
					800.0
				)
			; //

			tmp.fill(U,V);

		}

		static inline void
		WORK_QCD_TRAIN () {
			std::vector <
				std::string
			> U ; /* Get Loss name list: */ {
				U.push_back("./OUTS/QCD/TRAIN/0/predict");
				U.push_back("./OUTS/QCD/TRAIN/1/predict");
				U.push_back("./OUTS/QCD/TRAIN/2/predict");
				U.push_back("./OUTS/QCD/TRAIN/3/predict");
				U.push_back("./OUTS/QCD/TRAIN/4/predict");
				U.push_back("./OUTS/QCD/TRAIN/5/predict");
				U.push_back("./OUTS/QCD/TRAIN/6/predict");
				U.push_back("./OUTS/QCD/TRAIN/7/predict");
			}

			std::vector <
				std::string
			> V ; /* Get Vector name list: */ {
				V.push_back("./OUTS/QCD/TRAIN/0/vector");
				V.push_back("./OUTS/QCD/TRAIN/1/vector");
				V.push_back("./OUTS/QCD/TRAIN/2/vector");
				V.push_back("./OUTS/QCD/TRAIN/3/vector");
				V.push_back("./OUTS/QCD/TRAIN/4/vector");
				V.push_back("./OUTS/QCD/TRAIN/5/vector");
				V.push_back("./OUTS/QCD/TRAIN/6/vector");
				V.push_back("./OUTS/QCD/TRAIN/7/vector");
			}

			PLOT_LOSS_MASS
				tmp (
					"Mass_Loss_QCD_TRAIN" ,
					800.0
				)
			; //

			tmp.fill(U,V);

		}

		static inline void
		HUGE_DENCE_QCD () {

			/* TRAIN PART: */ {

				std::vector <
					std::string
				> U ; /* Get Loss name list: */ {
					U.push_back("./OUTS/QCD/TRAIN/0/predict_huge");
					U.push_back("./OUTS/QCD/TRAIN/1/predict_huge");
					U.push_back("./OUTS/QCD/TRAIN/2/predict_huge");
					U.push_back("./OUTS/QCD/TRAIN/3/predict_huge");
					U.push_back("./OUTS/QCD/TRAIN/4/predict_huge");
					U.push_back("./OUTS/QCD/TRAIN/5/predict_huge");
					U.push_back("./OUTS/QCD/TRAIN/6/predict_huge");
					U.push_back("./OUTS/QCD/TRAIN/7/predict_huge");
				}

				std::vector <
					std::string
				> V ; /* Get Vector name list: */ {
					V.push_back("./OUTS/QCD/TRAIN/0/vector");
					V.push_back("./OUTS/QCD/TRAIN/1/vector");
					V.push_back("./OUTS/QCD/TRAIN/2/vector");
					V.push_back("./OUTS/QCD/TRAIN/3/vector");
					V.push_back("./OUTS/QCD/TRAIN/4/vector");
					V.push_back("./OUTS/QCD/TRAIN/5/vector");
					V.push_back("./OUTS/QCD/TRAIN/6/vector");
					V.push_back("./OUTS/QCD/TRAIN/7/vector");
				}

				PLOT_LOSS_MASS
					tmp (
						"Mass_Loss_QCD_TRAIN_HUGE_DENCE" ,
						800.0
					)
				; //

				tmp.fill(U,V);

			}

			/* TEST PART: */ {

				std::vector <
					std::string
				> U ; /* Get Loss name list: */ {
					U.push_back("./OUTS/QCD/TEST/0/predict_huge");
					U.push_back("./OUTS/QCD/TEST/1/predict_huge");
					U.push_back("./OUTS/QCD/TEST/2/predict_huge");
					U.push_back("./OUTS/QCD/TEST/3/predict_huge");
					U.push_back("./OUTS/QCD/TEST/4/predict_huge");
					U.push_back("./OUTS/QCD/TEST/5/predict_huge");
					U.push_back("./OUTS/QCD/TEST/6/predict_huge");
					U.push_back("./OUTS/QCD/TEST/7/predict_huge");
				}

				std::vector <
					std::string
				> V ; /* Get Vector name list: */ {
					V.push_back("./OUTS/QCD/TEST/0/vector");
					V.push_back("./OUTS/QCD/TEST/1/vector");
					V.push_back("./OUTS/QCD/TEST/2/vector");
					V.push_back("./OUTS/QCD/TEST/3/vector");
					V.push_back("./OUTS/QCD/TEST/4/vector");
					V.push_back("./OUTS/QCD/TEST/5/vector");
					V.push_back("./OUTS/QCD/TEST/6/vector");
					V.push_back("./OUTS/QCD/TEST/7/vector");
				}

				PLOT_LOSS_MASS
					tmp (
						"Mass_Loss_QCD_TEST_HUGE_DENCE" ,
						800.0
					)
				; //

				tmp.fill(U,V);

			}

		}

		static inline void
		CNN_QCD_TRAIN () {
			std::vector <
				std::string
			> U ; /* Get Loss name list: */ {
				U.push_back("./OUTS/QCD/TRAIN/0/loss_cnn");
				U.push_back("./OUTS/QCD/TRAIN/1/loss_cnn");
				U.push_back("./OUTS/QCD/TRAIN/2/loss_cnn");
				U.push_back("./OUTS/QCD/TRAIN/3/loss_cnn");
				U.push_back("./OUTS/QCD/TRAIN/4/loss_cnn");
				U.push_back("./OUTS/QCD/TRAIN/5/loss_cnn");
				U.push_back("./OUTS/QCD/TRAIN/6/loss_cnn");
				U.push_back("./OUTS/QCD/TRAIN/7/loss_cnn");
			}

			std::vector <
				std::string
			> V ; /* Get Vector name list: */ {
				V.push_back("./OUTS/QCD/TRAIN/0/vector");
				V.push_back("./OUTS/QCD/TRAIN/1/vector");
				V.push_back("./OUTS/QCD/TRAIN/2/vector");
				V.push_back("./OUTS/QCD/TRAIN/3/vector");
				V.push_back("./OUTS/QCD/TRAIN/4/vector");
				V.push_back("./OUTS/QCD/TRAIN/5/vector");
				V.push_back("./OUTS/QCD/TRAIN/6/vector");
				V.push_back("./OUTS/QCD/TRAIN/7/vector");
			}

			PLOT_LOSS_MASS
				tmp (
					"Mass_Loss_QCD_TRAIN_cnn" ,
					800.0
				)
			; //

			tmp.fill(U,V);

		}

		static inline void
		CNN_QCD_TEST () {
			std::vector <
				std::string
			> U ; /* Get Loss name list: */ {
				U.push_back("./OUTS/QCD/TEST/0/loss_cnn");
				U.push_back("./OUTS/QCD/TEST/1/loss_cnn");
				U.push_back("./OUTS/QCD/TEST/2/loss_cnn");
				U.push_back("./OUTS/QCD/TEST/3/loss_cnn");
				U.push_back("./OUTS/QCD/TEST/4/loss_cnn");
				U.push_back("./OUTS/QCD/TEST/5/loss_cnn");
				U.push_back("./OUTS/QCD/TEST/6/loss_cnn");
				U.push_back("./OUTS/QCD/TEST/7/loss_cnn");
			}

			std::vector <
				std::string
			> V ; /* Get Vector name list: */ {
				V.push_back("./OUTS/QCD/TEST/0/vector");
				V.push_back("./OUTS/QCD/TEST/1/vector");
				V.push_back("./OUTS/QCD/TEST/2/vector");
				V.push_back("./OUTS/QCD/TEST/3/vector");
				V.push_back("./OUTS/QCD/TEST/4/vector");
				V.push_back("./OUTS/QCD/TEST/5/vector");
				V.push_back("./OUTS/QCD/TEST/6/vector");
				V.push_back("./OUTS/QCD/TEST/7/vector");
			}

			PLOT_LOSS_MASS
				tmp (
					"Mass_Loss_QCD_TEST_cnn" ,
					800.0
				)
			; //

			tmp.fill(U,V);

		}

		static inline void
		CNN_QCDHPT_FULL () {
			std::vector <
				std::string
			> U ; /* Get Loss name list: */ {
				U.push_back("./OUTS/QCD_HPT/TRAIN/0/loss_cnn");
				U.push_back("./OUTS/QCD_HPT/TRAIN/1/loss_cnn");
				U.push_back("./OUTS/QCD_HPT/TRAIN/2/loss_cnn");
				U.push_back("./OUTS/QCD_HPT/TRAIN/3/loss_cnn");
				U.push_back("./OUTS/QCD_HPT/TRAIN/4/loss_cnn");
				U.push_back("./OUTS/QCD_HPT/TRAIN/5/loss_cnn");
				U.push_back("./OUTS/QCD_HPT/TRAIN/6/loss_cnn");
				U.push_back("./OUTS/QCD_HPT/TRAIN/7/loss_cnn");
				U.push_back("./OUTS/QCD_HPT/TEST/0/loss_cnn");
				U.push_back("./OUTS/QCD_HPT/TEST/1/loss_cnn");
				U.push_back("./OUTS/QCD_HPT/TEST/2/loss_cnn");
				U.push_back("./OUTS/QCD_HPT/TEST/3/loss_cnn");
				U.push_back("./OUTS/QCD_HPT/TEST/4/loss_cnn");
				U.push_back("./OUTS/QCD_HPT/TEST/5/loss_cnn");
				U.push_back("./OUTS/QCD_HPT/TEST/6/loss_cnn");
				U.push_back("./OUTS/QCD_HPT/TEST/7/loss_cnn");
			}

			std::vector <
				std::string
			> V ; /* Get Vector name list: */ {
				V.push_back("./OUTS/QCD_HPT/TRAIN/0/vector");
				V.push_back("./OUTS/QCD_HPT/TRAIN/1/vector");
				V.push_back("./OUTS/QCD_HPT/TRAIN/2/vector");
				V.push_back("./OUTS/QCD_HPT/TRAIN/3/vector");
				V.push_back("./OUTS/QCD_HPT/TRAIN/4/vector");
				V.push_back("./OUTS/QCD_HPT/TRAIN/5/vector");
				V.push_back("./OUTS/QCD_HPT/TRAIN/6/vector");
				V.push_back("./OUTS/QCD_HPT/TRAIN/7/vector");
				V.push_back("./OUTS/QCD_HPT/TEST/0/vector");
				V.push_back("./OUTS/QCD_HPT/TEST/1/vector");
				V.push_back("./OUTS/QCD_HPT/TEST/2/vector");
				V.push_back("./OUTS/QCD_HPT/TEST/3/vector");
				V.push_back("./OUTS/QCD_HPT/TEST/4/vector");
				V.push_back("./OUTS/QCD_HPT/TEST/5/vector");
				V.push_back("./OUTS/QCD_HPT/TEST/6/vector");
				V.push_back("./OUTS/QCD_HPT/TEST/7/vector");
			}

			PLOT_LOSS_MASS
				tmp (
					"Mass_Loss_QCDHPT_FULL_cnn" ,
					8000.0
				)
			; //

			tmp.fill(U,V);

		}

		static inline void
		WORK_QCD () {
			WORK_QCD_TEST		() ;
			WORK_QCD_TRAIN		() ;
			HUGE_DENCE_QCD		() ;
			//WORK_QCDHPT_TEST	() ;
			//WORK_QCDHPT_TRAIN	() ;
			WORK_QCDHPT_FULL	() ;
			CNN_QCD_TRAIN		() ;
			CNN_QCD_TEST		() ;
			CNN_QCDHPT_FULL		() ;
		}


	} ;
}
////////////////////////////////////////////////////////////////
// EPSILON IN BINS OF MASS
namespace STEP11_SMALL_AE_LOSS_IN_MASS_BINS {

    using namespace
        MISC
    ; //

    // EPSILON IN BINS OF MASS
    class FC_AE_LOSS_ROC {
    public:

        using ROC_OUT_DATA =
            Tensors::Array::ND_ARRAY
            <2,TYPE_DATA>
        ;

        using names =
            std::vector <
                std::string
            >
        ;

        static inline void
        QCD_TRAIN_NAME_LIST (
            names & in
        ) {
            in.clear();
            in.push_back("./OUTS/QCD/TRAIN/0/predict");
            in.push_back("./OUTS/QCD/TRAIN/1/predict");
            in.push_back("./OUTS/QCD/TRAIN/2/predict");
            in.push_back("./OUTS/QCD/TRAIN/3/predict");
            in.push_back("./OUTS/QCD/TRAIN/4/predict");
            in.push_back("./OUTS/QCD/TRAIN/5/predict");
            in.push_back("./OUTS/QCD/TRAIN/6/predict");
            in.push_back("./OUTS/QCD/TRAIN/7/predict");
        }

        static inline void
        QCD_TRAIN_VECTOR_LIST (
            names & in
        ) {
            in.clear();
            in.push_back("./OUTS/QCD/TRAIN/0/vector");
            in.push_back("./OUTS/QCD/TRAIN/1/vector");
            in.push_back("./OUTS/QCD/TRAIN/2/vector");
            in.push_back("./OUTS/QCD/TRAIN/3/vector");
            in.push_back("./OUTS/QCD/TRAIN/4/vector");
            in.push_back("./OUTS/QCD/TRAIN/5/vector");
            in.push_back("./OUTS/QCD/TRAIN/6/vector");
            in.push_back("./OUTS/QCD/TRAIN/7/vector");
        }

        static inline void
        TOP_TRAIN_NAME_LIST (
            names & in
        ) {
            in.clear();
            in.push_back("./OUTS/TOP/TRAIN/0/predict");
            in.push_back("./OUTS/TOP/TRAIN/1/predict");
            in.push_back("./OUTS/TOP/TRAIN/2/predict");
            in.push_back("./OUTS/TOP/TRAIN/3/predict");
            in.push_back("./OUTS/TOP/TRAIN/4/predict");
            in.push_back("./OUTS/TOP/TRAIN/5/predict");
            in.push_back("./OUTS/TOP/TRAIN/6/predict");
            in.push_back("./OUTS/TOP/TRAIN/7/predict");
        }

        static inline void
        TOP_TRAIN_VECTOR_LIST (
            names & in
        ) {
            in.clear();
            in.push_back("./OUTS/TOP/TRAIN/0/vector");
            in.push_back("./OUTS/TOP/TRAIN/1/vector");
            in.push_back("./OUTS/TOP/TRAIN/2/vector");
            in.push_back("./OUTS/TOP/TRAIN/3/vector");
            in.push_back("./OUTS/TOP/TRAIN/4/vector");
            in.push_back("./OUTS/TOP/TRAIN/5/vector");
            in.push_back("./OUTS/TOP/TRAIN/6/vector");
            in.push_back("./OUTS/TOP/TRAIN/7/vector");
        }

        static inline void
        QCD_TEST_NAME_LIST (
            names & in
        ) {
            in.clear();
            in.push_back("./OUTS/QCD/TEST/0/predict");
            in.push_back("./OUTS/QCD/TEST/1/predict");
            in.push_back("./OUTS/QCD/TEST/2/predict");
            in.push_back("./OUTS/QCD/TEST/3/predict");
            in.push_back("./OUTS/QCD/TEST/4/predict");
            in.push_back("./OUTS/QCD/TEST/5/predict");
            in.push_back("./OUTS/QCD/TEST/6/predict");
            in.push_back("./OUTS/QCD/TEST/7/predict");
        }

        static inline void
        QCD_TEST_VECTOR_LIST (
            names & in
        ) {
            in.clear();
            in.push_back("./OUTS/QCD/TEST/0/vector");
            in.push_back("./OUTS/QCD/TEST/1/vector");
            in.push_back("./OUTS/QCD/TEST/2/vector");
            in.push_back("./OUTS/QCD/TEST/3/vector");
            in.push_back("./OUTS/QCD/TEST/4/vector");
            in.push_back("./OUTS/QCD/TEST/5/vector");
            in.push_back("./OUTS/QCD/TEST/6/vector");
            in.push_back("./OUTS/QCD/TEST/7/vector");
        }

        static inline void
        TOP_TEST_VECTOR_LIST (
            names & in
        ) {
            in.clear();
            in.push_back("./OUTS/TOP/TEST/0/vector");
            in.push_back("./OUTS/TOP/TEST/1/vector");
            in.push_back("./OUTS/TOP/TEST/2/vector");
            in.push_back("./OUTS/TOP/TEST/3/vector");
            in.push_back("./OUTS/TOP/TEST/4/vector");
            in.push_back("./OUTS/TOP/TEST/5/vector");
            in.push_back("./OUTS/TOP/TEST/6/vector");
            in.push_back("./OUTS/TOP/TEST/7/vector");
        }

        static inline void
        TOP_TEST_NAME_LIST (
            names & in
        ) {
            in.clear();
            in.push_back("./OUTS/TOP/TEST/0/predict");
            in.push_back("./OUTS/TOP/TEST/1/predict");
            in.push_back("./OUTS/TOP/TEST/2/predict");
            in.push_back("./OUTS/TOP/TEST/3/predict");
            in.push_back("./OUTS/TOP/TEST/4/predict");
            in.push_back("./OUTS/TOP/TEST/5/predict");
            in.push_back("./OUTS/TOP/TEST/6/predict");
            in.push_back("./OUTS/TOP/TEST/7/predict");
        }

        inline void
        StoreTOP () {
            ROC_OUT_DATA Results ;
            Results[0] = mass ;
            Results[1] = loss ;
            TOP_Writer.push_back(Results);
            Hists.Fill<1>(loss);
            signalTree->Fill();
        }

        inline void
        StoreQCD () {
            ROC_OUT_DATA Results ;
            Results[0] = mass ;
            Results[1] = loss ;
            QCD_Writer.push_back(Results);
            Hists.Fill<0>(loss);
            background->Fill();
        }

        inline void
        PLOT_QCD () {

            names list , list2 ;
            QCD_TEST_NAME_LIST		(list)	;
            QCD_TEST_VECTOR_LIST	(list2)	;

            for(size_t j=0;j<list.size();j++){

                CPPFileIO::FullFileReader
                    <TYPE_DATA>
                    Reader (list[j])
                ; //

                CPPFileIO::FullFileReader
                    <outvector4>
                    Reader2 (list2[j])
                ; //

                size_t const min =
                    CPPFileIO::mymin (
                        Reader()	,
                        Reader2()
                    )
                ; //

                for(size_t i=0;i<min;i++) {
                    loss = Reader(i) ;
                    mass = Reader2(i).m() ;
                    StoreQCD () ;
                }

            }

        }

        inline void
        PLOT_TOP () {

            names list , list2 ;
            TOP_TEST_NAME_LIST		(list)	;
            TOP_TEST_VECTOR_LIST	(list2)	;

            for(size_t j=0;j<list.size();j++){

                CPPFileIO::FullFileReader
                    <TYPE_DATA>
                    Reader (list[j])
                ; //

                CPPFileIO::FullFileReader
                    <outvector4>
                    Reader2 (list2[j])
                ; //

                size_t const min =
                    CPPFileIO::mymin (
                        Reader()	,
                        Reader2()
                    )
                ; //

                for(size_t i=0;i<min;i++) {
                    loss = Reader(i) ;
                    mass = Reader2(i).m() ;
                    StoreTOP () ;
                }

            }
        }

        inline void
        PLOT_QCD_TRAIN () {

            names list , list2 ;
            QCD_TRAIN_NAME_LIST		(list)	;
            QCD_TRAIN_VECTOR_LIST	(list2)	;

            for(size_t j=0;j<list.size();j++){

                CPPFileIO::FullFileReader
                    <TYPE_DATA>
                    Reader (list[j])
                ; //

                CPPFileIO::FullFileReader
                    <outvector4>
                    Reader2 (list2[j])
                ; //

                size_t const min =
                    CPPFileIO::mymin (
                        Reader()	,
                        Reader2()
                    )
                ; //

                for(size_t i=0;i<min;i++) {
                    loss = Reader(i) ;
                    mass = Reader2(i).m() ;
                    Hists.Fill<2>(loss);
                }

            }

        }

        inline void
        PLOT_TOP_TRAIN () {

            names list , list2 ;
            TOP_TRAIN_NAME_LIST		(list)	;
            TOP_TRAIN_VECTOR_LIST	(list2)	;

            for(size_t j=0;j<list.size();j++){

                CPPFileIO::FullFileReader
                    <TYPE_DATA>
                    Reader (list[j])
                ; //

                CPPFileIO::FullFileReader
                    <outvector4>
                    Reader2 (list2[j])
                ; //

                size_t const min =
                    CPPFileIO::mymin (
                        Reader()	,
                        Reader2()
                    )
                ; //

                for(size_t i=0;i<min;i++) {
                    loss = Reader(i) ;
                    mass = Reader2(i).m() ;
                    Hists.Fill<3>(loss);
                }

            }

        }

		static inline void
		PLOT_QCD_MASS_LOSS_BINS () {

			MyHistN <6,false>
				CNN_BINS (
					"SHORT_FC_AE_LOSS_BINS",
					100,-0.01,0.61
				)
			; //

			CNN_BINS.fixedy = true ;

			names	list	,	list2	;
			QCD_TRAIN_NAME_LIST		(list)	;
			QCD_TRAIN_VECTOR_LIST	(list2)	;

			size_t constexpr masses[7] =
				{0,100,200,300,400,500,600}
			;

			for(size_t j=0;j<list.size();j++){

				CPPFileIO::FullFileReader
					<TYPE_DATA>
					Reader (list[j])
				; //

				CPPFileIO::FullFileReader
					<outvector4>
					Reader2 (list2[j])
				; //

				size_t const min =
					CPPFileIO::mymin (
						Reader()	,
						Reader2()
					)
				; //

				for(size_t i=0;i<min;i++) {
					TYPE_DATA loss = Reader(i) ;
					TYPE_DATA mass = Reader2(i).m() ;
					if (false) {
						if      ((masses[0]<=mass)&&(mass<masses[1])) {CNN_BINS.Fill<0>(loss);}
						else if ((masses[1]<=mass)&&(mass<masses[2])) {CNN_BINS.Fill<1>(loss);}
						else if ((masses[2]<=mass)&&(mass<masses[3])) {CNN_BINS.Fill<2>(loss);}
						else if ((masses[3]<=mass)&&(mass<masses[4])) {CNN_BINS.Fill<3>(loss);}
						else if ((masses[4]<=mass)&&(mass<masses[5])) {CNN_BINS.Fill<4>(loss);}
						else if ((masses[5]<=mass)&&(mass<masses[6])) {CNN_BINS.Fill<5>(loss);}
						else if (masses[6]<=mass)                     {CNN_BINS.Fill<6>(loss);}
					} else if (true) {
						if      ((masses[0]<=mass)&&(mass<masses[1]))	{CNN_BINS.Fill<0>(loss);}
						else if ((masses[1]<=mass)&&(mass<masses[2]))	{CNN_BINS.Fill<1>(loss);}
						else if ((masses[2]<=mass)&&(mass<masses[3]))	{CNN_BINS.Fill<2>(loss);}
						else if ((masses[3]<=mass)&&(mass<masses[4]))	{CNN_BINS.Fill<3>(loss);}
						else if ((masses[4]<=mass)&&(mass<masses[5]))	{CNN_BINS.Fill<4>(loss);}
						else if ((masses[5]<=mass))						{CNN_BINS.Fill<5>(loss);}
					}
				}

			}

		}


        FC_AE_LOSS_ROC () :
        Hists ("Losses_SHORT_FC_AE",100,-0.01,0.3) ,
        QCD_Writer ("./OUTS/ROC/QCD_FC_AE_DATA") ,
        TOP_Writer ("./OUTS/ROC/TOP_FC_AE_DATA") {
			Hists.fixedy = true ;

            /* Prepare the directories: */ {
                mkdir("./OUTS/",0755);
                mkdir("./OUTS/TMP/",0755);
                mkdir("./OUTS/TMP/tmva/",0755);
            }

            /* PREPARE ROOT FILE: */ {
                Outfile =
                    new TFile (
                        "./OUTS/TMP/tmva/CNN_LOSS_NORMAL.root",
                        "RECREATE"
                    )
                ;
            }

            /* PREPARE BACKGROUND TREE: */ {
                background =
                    new TTree (
                        "TreeB",
                        "TreeB"
                    )
                ; //
                background->Branch ( "mass" , & mass ) ;
                background->Branch ( "loss" , & loss ) ;
            }

            /* PREPARE SIGNAL TREE: */ {
                signalTree =
                    new TTree (
                        "TreeS",
                        "TreeS"
                    )
                ; //
                signalTree->Branch ( "mass" , & mass ) ;
                signalTree->Branch ( "loss" , & loss ) ;
            }

            PLOT_QCD();
            PLOT_TOP();
            //PLOT_QCD_TRAIN();
            //PLOT_TOP_TRAIN();

        }

        ~FC_AE_LOSS_ROC(){
            signalTree->Write()	;
            background->Write()	;
            delete signalTree	;
            delete background	;
            delete Outfile		;
        }

        MyHistN <4,true>
            Hists
        ; //

        TFile *	Outfile		;
        TTree *	signalTree	;
        TTree *	background	;
        float	loss		;
        float	mass		;

        CPPFileIO::FileVector
        <ROC_OUT_DATA>
            QCD_Writer	,
            TOP_Writer
        ;

        static inline void
        WORK ()
        { FC_AE_LOSS_ROC tmp ; PLOT_QCD_MASS_LOSS_BINS(); }

    } ;

}
////////////////////////////////////////////////////////////////
namespace STEP12_MASS_PT_OF_TOP_W_H4J {

	using namespace MISC ;

	class Plotter {
	public:

		static inline names
		QCDNames () {
			names ret ;
			ret.push_back("./OUTS/QCD/TEST/0/vector");
			ret.push_back("./OUTS/QCD/TEST/1/vector");
			ret.push_back("./OUTS/QCD/TEST/2/vector");
			ret.push_back("./OUTS/QCD/TEST/3/vector");
			ret.push_back("./OUTS/QCD/TEST/4/vector");
			ret.push_back("./OUTS/QCD/TEST/5/vector");
			ret.push_back("./OUTS/QCD/TEST/6/vector");
			ret.push_back("./OUTS/QCD/TEST/7/vector");
			return ret ;
		}

		static inline names
		TopNames () {
			names ret ;
			ret.push_back("./OUTS/TOP/TEST/0/vector");
			ret.push_back("./OUTS/TOP/TEST/1/vector");
			ret.push_back("./OUTS/TOP/TEST/2/vector");
			ret.push_back("./OUTS/TOP/TEST/3/vector");
			ret.push_back("./OUTS/TOP/TEST/4/vector");
			ret.push_back("./OUTS/TOP/TEST/5/vector");
			ret.push_back("./OUTS/TOP/TEST/6/vector");
			ret.push_back("./OUTS/TOP/TEST/7/vector");
			return ret ;
		}

		static inline names
		WBSNames () {
			names ret ;
			ret.push_back("./OUTS/WBS/TEST/0/vector");
			ret.push_back("./OUTS/WBS/TEST/1/vector");
			ret.push_back("./OUTS/WBS/TEST/2/vector");
			ret.push_back("./OUTS/WBS/TEST/3/vector");
			ret.push_back("./OUTS/WBS/TEST/4/vector");
			ret.push_back("./OUTS/WBS/TEST/5/vector");
			ret.push_back("./OUTS/WBS/TEST/6/vector");
			ret.push_back("./OUTS/WBS/TEST/7/vector");
			return ret ;
		}

		static inline names
		H4JNames () {
			names ret ;
			ret.push_back("./OUTS/H4J/TEST/0/vector");
			ret.push_back("./OUTS/H4J/TEST/1/vector");
			ret.push_back("./OUTS/H4J/TEST/2/vector");
			ret.push_back("./OUTS/H4J/TEST/3/vector");
			ret.push_back("./OUTS/H4J/TEST/4/vector");
			ret.push_back("./OUTS/H4J/TEST/5/vector");
			ret.push_back("./OUTS/H4J/TEST/6/vector");
			ret.push_back("./OUTS/H4J/TEST/7/vector");
			return ret ;
		}

		static inline READER_Vector
		TopReader (size_t i) {
			READER_Vector ret (TopNames()[i]) ;
			return ret ;
		}

		static inline READER_Vector
		WBSReader (size_t i) {
			READER_Vector ret (WBSNames()[i]) ;
			return ret ;
		}

		static inline READER_Vector
		H4JReader (size_t i) {
			READER_Vector ret (H4JNames()[i]) ;
			return ret ;
		}

		static inline READER_Vector
		QCDReader (size_t i) {
			READER_Vector ret (QCDNames()[i]) ;
			return ret ;
		}

//////////////////////////////////////////
#define _MACRO_PLOTTER_(Name,Index)		\
	auto out = Name(j) ;				\
	for(size_t i=0;i<out();i++){		\
		PTs.Fill<Index>(out(i).pt());	\
		MJs.Fill<Index>(out(i).m());	\
	}									//
//////////////////////////////////////////
		inline void
		Plot () {
			for(size_t j=0;j<8;j++){
				/* QCD PART: */ {
					_MACRO_PLOTTER_(QCDReader,0)
				}
				/* TOP PART: */ {
					_MACRO_PLOTTER_(TopReader,1)
				}
				/* WBS PART: */ {
					_MACRO_PLOTTER_(WBSReader,2)
				}
				/* H4J PART: */ {
					_MACRO_PLOTTER_(H4JReader,3)
				}
			}
		}
#undef _MACRO_PLOTTER_

		Plotter():
		PTs("PT_OF_QCD_TOP_W_H4J",100,780,920),
		MJs("MJ_OF_QCD_TOP_W_H4J",100,0.0,600)
		{ Plot () ; }

		~Plotter(){}

		MyHistN <4> PTs ;
		MyHistN <4> MJs ;

		static inline void WORK () {
			auto * tmp = new Plotter ;
			delete tmp ;
		}

	} ;
}
////////////////////////////////////////////////////////////////
