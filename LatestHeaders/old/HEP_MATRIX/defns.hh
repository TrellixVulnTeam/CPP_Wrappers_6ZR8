		////////////////////////////////
		// The Definitions /////////////
		////////////////////////////////
		using TYPE_DATA =
			T
		; //
		using TYPE_COMPLEX_DATA =
			std::complex <TYPE_DATA>
		; //
		using TYPE_MAIN_VECTOR =
			Tensors::NN::ND_ARRAY
				<4,TYPE_COMPLEX_DATA>
		; //
		using TYPE_MAIN_MATRIX =
			Tensors::NN::N2D_ARRAY
				<4,4,TYPE_COMPLEX_DATA>
		; //
		using TYPE_REAL_MATRIX =
			Tensors::NN::N2D_ARRAY
				<4,4,TYPE_DATA>
		; //
		using TYPE_FULL_GAMMA_MATRIX =
			Tensors::NN::ND_ARRAY
				<5,TYPE_MAIN_MATRIX>
		; //
		using TYPE_LOOP_RESULTS =
			LoopResultStorage
				<TYPE_DATA>
		; //
		////////////////////////////////
		// Important Constants: ////////
		////////////////////////////////
		static inline TYPE_DATA constexpr
		PI() {
			return
				3.1415926535897932384626433
			;
		} //
		static inline TYPE_DATA constexpr
		CHARGE_E() {
			return
				0.30282212
			;
		} //
		static inline TYPE_DATA constexpr
		EPS() {
			return
				0.000000001
			;
		} //
		static inline size_t constexpr
		FACTORIAL (size_t const i) {
			if (i<=1) {
				return
					1
				; //
			} else {
				return
					FACTORIAL(i-1)*i
				; //
			}
		} //
		static inline TYPE_DATA constexpr
		HARMONIC_SUM (size_t const i) {
			if (i<=1) {
				return
					1
				; //
			} else {
				return
					HARMONIC_SUM(i-1) +
					(
						1.0 /
						static_cast <TYPE_DATA> (i)
					)
				; //
			}
		} //
		static inline long constexpr
		MINUS_1 (long n) {
			if(n==0){return 1;}
			else if(n>0){return -MINUS_1(n-1);}
			else if(n<0){return -MINUS_1(n+1);}
		} //
		static inline TYPE_DATA constexpr
		EULER_GAMMA () {
			// https://en.wikipedia.org/wiki/Euler%E2%80%93Mascheroni_constant
			return
				0.57721566490153286060651209008240243104215933593
			; //
		}
		////////////////////////////////
		// Complexified constants: /////
		////////////////////////////////
		static inline TYPE_COMPLEX_DATA
		TO_C (TYPE_DATA const in) {
			TYPE_COMPLEX_DATA
				ret (in,0)
			; //
			return
				ret
			; //
		}
		//
		static inline TYPE_COMPLEX_DATA constexpr
		C_1 () {
			return
				TYPE_COMPLEX_DATA
					( 1.0 , 0.0 )
			; //
		}
		//
		static inline TYPE_COMPLEX_DATA constexpr
		C_I () {
			return
				TYPE_COMPLEX_DATA
					( 0.0 , 1.0 )
			; //
		}
		//
		static inline TYPE_COMPLEX_DATA constexpr
		IEPS () {
			return
				C_I () * EPS()
			; //
		}
		//
		static inline TYPE_COMPLEX_DATA constexpr
		C_PI () {
			return
				C_I() * PI()
			; //
		}
		//
		static inline TYPE_COMPLEX_DATA constexpr
		C_E () {
			return
				C_I() * CHARGE_E()
			; //
		}
		////////////////////////////////
		// Function to get metric //////
		////////////////////////////////
		static inline TYPE_REAL_MATRIX constexpr
		METRIC () {
			TYPE_REAL_MATRIX ret ;
			ret       =  0.0 ;
			ret[0][0] =  1.0 ;
			ret[1][1] = -1.0 ;
			ret[2][2] = -1.0 ;
			ret[3][3] = -1.0 ;
			return ret ;
		}
		////////////////////////////////
		// Simple Functions: ///////////
		////////////////////////////////
		static inline TYPE_COMPLEX_DATA
		DAGGER (
			TYPE_COMPLEX_DATA const
				in
		) {
			return
				std::conj (in)
			;
		} //
		static inline TYPE_MAIN_VECTOR
		GET_VECTOR (
			TYPE_DATA	const	En	,
			TYPE_DATA	const	X	,
			TYPE_DATA	const	Y	,
			TYPE_DATA	const	Z
		) {
			TYPE_MAIN_VECTOR
				ret
			; /* Prepare the vector */ {
				ret[0] =
					TYPE_COMPLEX_DATA
						( En , 0 )
				; //
				ret[1] =
					TYPE_COMPLEX_DATA
						( X  , 0 )
				; //
				ret[2] =
					TYPE_COMPLEX_DATA
						( Y  , 0 )
				; //
				ret[3] =
					TYPE_COMPLEX_DATA
						( Z  , 0 )
				; //
			} //
			return
				ret
			; //
		} //
		static inline TYPE_MAIN_VECTOR
		GET_VECTOR (
			TYPE_DATA const M ,
			TYPE_DATA const Z
		) {
			TYPE_DATA En =
				std::sqrt (
					(Z*Z) +
					(M*M)
				)
			;
			return
				GET_VECTOR(
					En	,	0 ,
					0	,	Z
				)
			;
		}
		//
		static inline TYPE_MAIN_VECTOR
		DAGGER (
			TYPE_MAIN_VECTOR const &
				in
		) {
			TYPE_MAIN_VECTOR
				ret
			; //
			for(size_t i=0;i<in.SIZE();i++){
				ret[i] =
					DAGGER (in[i])
				; //
			}
			return
				ret
			; //
		}
		//
		static inline TYPE_MAIN_MATRIX
		DAGGER (
			TYPE_MAIN_MATRIX const &
				in
		) {
			TYPE_MAIN_MATRIX
				ret
			; //
			for(size_t y=0;y<ret.SIZE_Y();y++){
				for(size_t x=0;x<ret.SIZE_X();x++){
					ret[y][x] =
						DAGGER (
							in[x][y]
						)
					; //
				}
			}
			return
				ret
			; //
		}
		//
		static inline TYPE_COMPLEX_DATA
		DOT (
			TYPE_MAIN_VECTOR const & p1 ,
			TYPE_MAIN_VECTOR const & p2
		) {
			return
				(p1[0]*DAGGER(p2[0])) -
				(p1[1]*DAGGER(p2[1])) -
				(p1[2]*DAGGER(p2[2])) -
				(p1[3]*DAGGER(p2[3]))
			; //
		}
		//
		static inline TYPE_COMPLEX_DATA
		NORM2 (
			TYPE_MAIN_VECTOR const &
				p
		) {
			return
				DOT(p,p)
			; //
		}
		////////////////////////////////
		// Scalar Propagator: //////////
		////////////////////////////////
		static inline TYPE_COMPLEX_DATA
		PROPAGATOR (
			TYPE_DATA const & p2         ,
			TYPE_DATA const   m  = 0     ,
			TYPE_DATA const   w  = EPS()
		) {
			return C_I() / (
				p2 +
				TYPE_COMPLEX_DATA
					(-(m*m),w)
			) ;
			//
		}
		//
		static inline TYPE_COMPLEX_DATA
		PROPAGATOR (
			TYPE_MAIN_VECTOR const & p         ,
			TYPE_DATA        const   m = 0     ,
			TYPE_DATA        const   w = EPS()
		) {
			return
				PROPAGATOR (
					NORM2(p).real() ,
					m , w
				)
			;
		}
		////////////////////////////////
		// More convinence functions: //
		////////////////////////////////
		static inline TYPE_MAIN_MATRIX constexpr
		ZERO_MATRIX () {
			TYPE_MAIN_MATRIX
				ret
			; //
			ret =
				TYPE_COMPLEX_DATA ( 0 , 0 )
			; //
			return
				ret
			; //
		}
		//
		static inline TYPE_MAIN_VECTOR constexpr
		ZERO_VECTOR () {
			TYPE_MAIN_VECTOR
				ret (
					TYPE_COMPLEX_DATA
						( 0 , 0 )
				)
			;
			return
				ret
			;
		}
		//
		static inline TYPE_MAIN_MATRIX constexpr
		IDENTITY () {
			auto ret = ZERO_MATRIX () ;
			ret[0][0] = TYPE_COMPLEX_DATA ( 1 , 0 ) ;
			ret[1][1] = TYPE_COMPLEX_DATA ( 1 , 0 ) ;
			ret[2][2] = TYPE_COMPLEX_DATA ( 1 , 0 ) ;
			ret[3][3] = TYPE_COMPLEX_DATA ( 1 , 0 ) ;
			return ret ;
		}
		////////////////////////////////
		// The Gamma Matrix Part ///////
		////////////////////////////////
		static inline TYPE_MAIN_MATRIX constexpr
		C_METRIC() {
			TYPE_MAIN_MATRIX ret = ZERO_MATRIX () ;
			ret[0][0] = TYPE_COMPLEX_DATA (+1.0,0.0) ;
			ret[1][1] = TYPE_COMPLEX_DATA (-1.0,0.0) ;
			ret[2][2] = TYPE_COMPLEX_DATA (-1.0,0.0) ;
			ret[3][3] = TYPE_COMPLEX_DATA (-1.0,0.0) ;
			return ret ;
		}
		//
		static inline TYPE_MAIN_MATRIX constexpr
		GAMMA_0 () {
			TYPE_MAIN_MATRIX
				ret
			;
			ret = TYPE_COMPLEX_DATA ( 0 , 0 ) ;
			ret[0][2] = TYPE_COMPLEX_DATA ( 1 , 0 ) ;
			ret[1][3] = TYPE_COMPLEX_DATA ( 1 , 0 ) ;
			ret[2][0] = TYPE_COMPLEX_DATA ( 1 , 0 ) ;
			ret[3][1] = TYPE_COMPLEX_DATA ( 1 , 0 ) ;
			return ret ;
		}
		//
		static inline TYPE_MAIN_MATRIX constexpr
		GAMMA_1 () {
			TYPE_MAIN_MATRIX
				ret
			;
			ret = TYPE_COMPLEX_DATA ( 0 , 0 ) ;
			ret[0][3] = TYPE_COMPLEX_DATA (  1 , 0 ) ;
			ret[1][2] = TYPE_COMPLEX_DATA (  1 , 0 ) ;
			ret[2][1] = TYPE_COMPLEX_DATA ( -1 , 0 ) ;
			ret[3][0] = TYPE_COMPLEX_DATA ( -1 , 0 ) ;
			return ret ;
		}
		//
		static inline TYPE_MAIN_MATRIX constexpr
		GAMMA_2 () {
			TYPE_MAIN_MATRIX
				ret
			;
			ret = TYPE_COMPLEX_DATA ( 0 , 0 ) ;
			ret[0][3] = TYPE_COMPLEX_DATA ( 0 , -1 ) ;
			ret[1][2] = TYPE_COMPLEX_DATA ( 0 ,  1 ) ;
			ret[2][1] = TYPE_COMPLEX_DATA ( 0 ,  1 ) ;
			ret[3][0] = TYPE_COMPLEX_DATA ( 0 , -1 ) ;
			return ret ;
		}
		//
		static inline TYPE_MAIN_MATRIX constexpr
		GAMMA_3 () {
			TYPE_MAIN_MATRIX
				ret
			;
			ret = TYPE_COMPLEX_DATA ( 0 , 0 ) ;
			ret[0][2] = TYPE_COMPLEX_DATA (  1 , 0 ) ;
			ret[1][3] = TYPE_COMPLEX_DATA ( -1 , 0 ) ;
			ret[2][0] = TYPE_COMPLEX_DATA ( -1 , 0 ) ;
			ret[3][1] = TYPE_COMPLEX_DATA (  1 , 0 ) ;
			return ret ;
		}
		//
		static inline TYPE_MAIN_MATRIX constexpr
		GAMMA_5 () {
			TYPE_MAIN_MATRIX
				ret
			;
			ret = TYPE_COMPLEX_DATA ( 0 , 0 ) ;
			ret[0][0] = TYPE_COMPLEX_DATA ( -1 , 0 ) ;
			ret[1][1] = TYPE_COMPLEX_DATA ( -1 , 0 ) ;
			ret[2][2] = TYPE_COMPLEX_DATA (  1 , 0 ) ;
			ret[3][3] = TYPE_COMPLEX_DATA (  1 , 0 ) ;
			return ret ;
		}
		////////////////////////////////
		// Projection Operators: ///////
		////////////////////////////////
		static inline TYPE_MAIN_MATRIX constexpr
		PR () {
			auto G5 = GAMMA_5 () ;
			TYPE_MAIN_MATRIX ret ;
			for(size_t y=0;y<ret.SIZE_Y();y++){
				for(size_t x=0;x<ret.SIZE_X();x++){
					if (y==x) {
						ret[y][x] = (
							+ TYPE_COMPLEX_DATA (1.0,0.0)
							+ G5[y][x]
						) / 2.0 ;
					} else {
						ret[y][x] = (
							+ G5[y][x]
						) / 2.0 ;
					}
				}
			}
			return ret ;
		}
		//
		static inline TYPE_MAIN_MATRIX constexpr
		PL () {
			auto G5 = GAMMA_5 () ;
			TYPE_MAIN_MATRIX ret ;
			for(size_t y=0;y<ret.SIZE_Y();y++){
				for(size_t x=0;x<ret.SIZE_X();x++){
					if (y==x) {
						ret[y][x] = (
							+ TYPE_COMPLEX_DATA (1.0,0.0)
							- G5[y][x]
						) / 2.0 ;
					} else {
						ret[y][x] = (
							- G5[y][x]
						) / 2.0 ;
					}
				}
			}
			return ret ;
		}
		//
		static inline TYPE_FULL_GAMMA_MATRIX constexpr
		ALL_GAMMA () {
			TYPE_FULL_GAMMA_MATRIX ret ;
			ret[0] = GAMMA_0 () ;
			ret[1] = GAMMA_1 () ;
			ret[2] = GAMMA_2 () ;
			ret[3] = GAMMA_3 () ;
			ret[4] = GAMMA_5 () ;
			return ret ;
		}
		////////////////////////////////
		// QED Vertex: /////////////////
		////////////////////////////////
		static inline TYPE_FULL_GAMMA_MATRIX constexpr
		QED_VERTEX () {
			auto tmp =
				TYPE_COMPLEX_DATA
					(0.0,-1.0*CHARGE_E())
			;
			//
			TYPE_FULL_GAMMA_MATRIX ret ;
			ret[0] = MULTIPLY(GAMMA_0(),tmp);
			ret[1] = MULTIPLY(GAMMA_1(),tmp);
			ret[2] = MULTIPLY(GAMMA_2(),tmp);
			ret[3] = MULTIPLY(GAMMA_3(),tmp);
			ret[4] = GAMMA_5();
			return ret ;
		}
		////////////////////////////////
		// Functions on Matrices ///////
		////////////////////////////////
		static inline TYPE_MAIN_MATRIX
		GET_SLASH_ACCURATE (
			TYPE_MAIN_VECTOR const &
				p
		) {
			TYPE_MAIN_MATRIX ret ;
			for(size_t y=0;y<ret.SIZE_Y();y++){
				for(size_t x=0;x<ret.SIZE_X();x++){
					ret[y][x] = 0 ;
					for(size_t i=0;i<4;i++){
						ret[y][x] +=
							ALL_GAMMA()[i][y][x] *
							p[i]
						; //
					}
				}
			}
			return ret ;
		}
		//
		static inline TYPE_MAIN_MATRIX
		GET_SLASH_FAST (
			TYPE_MAIN_VECTOR const &
				p
		) {
			TYPE_MAIN_MATRIX ret =
				ZERO_MATRIX()
			; //
			ret[0][2] =   p[3] + p[0] ;
			ret[0][3] =   p[1] - (p[2]*C_I()) ;
			//
			ret[1][2] =   p[1] + (p[2]*C_I()) ;
			ret[1][3] = - p[3] + p[0] ;
			//
			ret[2][0] = - p[3] + p[0] ;
			ret[2][1] = - p[1] + (p[2]*C_I()) ;
			//
			ret[3][0] = - p[1] - (p[2]*C_I()) ;
			ret[3][1] =   p[3] + p[0] ;
			//
			return ret ;
			//
		}
		//
		static inline TYPE_MAIN_MATRIX
		GET_SLASH (
			TYPE_MAIN_VECTOR const &
				p
		) {
			return
				GET_SLASH_FAST
					(p)
			;
		}
		////////////////////////////////
		static inline TYPE_MAIN_MATRIX
		FERMION_NUMERATOR_U (
			TYPE_MAIN_VECTOR const & p ,
			TYPE_DATA        const   m
		) {
			return
				ADD (
					GET_SLASH(p),
					MULTIPLY(IDENTITY(),m)
				)
			;
		}
		//
		static inline TYPE_MAIN_MATRIX
		FERMION_NUMERATOR_V (
			TYPE_MAIN_VECTOR const & p ,
			TYPE_DATA        const   m
		) {
			return
				SUBTRACT (
					GET_SLASH(p),
					MULTIPLY(IDENTITY(),m)
				)
			; //
		}
		//
		static inline TYPE_MAIN_MATRIX
		FERMION_PROPAGATOR_U (
			TYPE_MAIN_VECTOR const & p         ,
			TYPE_DATA        const   m = 0     ,
			TYPE_DATA        const   w = EPS()
		) {
			return
				MULTIPLY (
					FERMION_NUMERATOR_U(p,m),
					PROPAGATOR(p,m,w)
				)
			;
		}
		//
		static inline TYPE_MAIN_MATRIX
		FERMION_PROPAGATOR (
			TYPE_MAIN_VECTOR const & p         ,
			TYPE_DATA        const   m = 0     ,
			TYPE_DATA        const   w = EPS()
		) {
			return
				FERMION_PROPAGATOR_U
					(p,m,w)
			; //
		}
		////////////////////////////////////////////////////////////////
		// Vector propagator = -g_mu_nu * PROPAGATOR ///////////////////
		////////////////////////////////////////////////////////////////
		static inline TYPE_MAIN_MATRIX
		PHOTON_PROPAGATOR (
			TYPE_MAIN_VECTOR const &
				p
		) {
			auto k = PROPAGATOR (p) ;
			TYPE_MAIN_MATRIX ret =
				ZERO_MATRIX ()
			; /* Prepare the matrix: */ {
				ret[0][0] = -1 ;
				ret[1][1] =  1 ;
				ret[2][2] =  1 ;
				ret[3][3] =  1 ;
			}
			return
				MULTIPLY (ret,k)
			; //
		}
		////////////////////////////////////////////////////////////////
