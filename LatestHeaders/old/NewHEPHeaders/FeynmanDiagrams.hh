namespace NewHEPHeaders /* Wrappers to Draw feynman diagrams: */ {
	class FeynDrawer {
	public:

		using TYPE_DATA = double ;

		using vector2 =
			VECTORS::plane2vector
				<TYPE_DATA>
		; //

		using vector2s =
			std::vector
				<vector2>
		; //

		using D2Point =
			struct {
				TYPE_DATA X, Y ;
			}
		;

		inline static double constexpr
		PI2 () { return  CONSTANTS::PI2 ; }

		static inline vector2
		perp (
			vector2 const &
				in
		) {
			vector2 ret (-in[1],in[0]) ;
			return ret ;
		}
		inline void
		show (
			vector2 const &
				in
		) {
			fprintf (
				f,
				"%e %e\n",
				in[0], in[1]
			) ;
		}
		inline void
		newline ()
		{fprintf(f,"\n");}

		inline void
		LINE (
			vector2 const a ,
			vector2 const b
		) {
			show(a);
			show(b);
			newline () ;
		}

		inline void
		DASHED_SEGMENT (
			vector2 const a ,
			vector2 const b
		) {
			vector2 const
				c1 =
					(
						( a * static_cast<TYPE_DATA>(2) ) +
						b
					) / static_cast<TYPE_DATA>(3)
			; //
			vector2 const
				c2 =
					(
						a +
						( b * static_cast<TYPE_DATA>(2) )
					) / static_cast<TYPE_DATA>(3)
			; //
			LINE(a,c1);newline();
			LINE(c2,b);newline();
		}
		inline void
		DASHED_LINE (
			vector2	const begin	,
			vector2	const end	,
			size_t	const n = 3
		) {
			vector2 const mid =
				(end + begin) /
				static_cast<TYPE_DATA>(2)
			; //
			if(n>1){
				DASHED_LINE(begin,mid,n-1);
				DASHED_LINE(mid,end,n-1);
			} else if(n==1){
				DASHED_SEGMENT(begin,mid);
				DASHED_SEGMENT(mid,end);
			}
		}
// 		inline void
// 		DASHED_LINE (
// 			D2Point	const a ,
// 			D2Point	const b ,
// 			size_t	const n = 3
// 		) {
// 			vector2 const begin	(a.X,a.Y) ;
// 			vector2 const end	(b.X,b.Y) ;
// 			vector2 const mid =
// 				(end + begin) /
// 				static_cast<TYPE_DATA>(2)
// 			; //
// 			DASHED_LINE(begin,end,n);
// 		}

		inline void
		WAVY_SEGMENT (
			vector2	const x1		,
			vector2	const x2		,
			size_t	const n = 100	,
			size_t	const s = 4
		) {
			vector2 const line = x2 - x1 ;
			vector2 const dx =
				line /
				static_cast<TYPE_DATA>(n)
			; //

			auto const magnitude = line.pt() / static_cast<TYPE_DATA>(s) ;

			auto P = perp((x2-x1).dir());

			TYPE_DATA const dy =
				CONSTANTS::PI *
				static_cast<TYPE_DATA>(2) /
				static_cast<TYPE_DATA>(n)
			; //

			for(size_t i=0;i<n;i++){
				TYPE_DATA angle = i * dy ;
				auto tmp_p =
					P * magnitude *
					static_cast<TYPE_DATA>(std::sin(angle))
				;
				vector2 const x =
					x1 +
					(dx*static_cast<TYPE_DATA>(i))
				; //
				show(x+tmp_p);
			}
			show(x2);
		}
		inline void
		WAVY_LINE (
			vector2	const x1 ,
			vector2	const x2 ,
			size_t	const n = 3
		) {
			vector2 const mid =
				(x1 + x2) /
				static_cast<TYPE_DATA>(2)
			; //
			if (n>1) {
				WAVY_LINE(x1,mid,n-1);
				WAVY_LINE(mid,x2,n-1);
			} else if (n==1) {
				WAVY_SEGMENT(x1,x2,100);
			}
		}

		inline void
		ARROW (
			vector2 const x1 ,
			vector2 const x2
		) {
			auto const
				X =
					x2 - x1
			; //
			auto const
				Y = perp(X)
			; //
			auto const
				ZERO = x1
			; //
			auto const
				p1 =
					ZERO +
					(Y*0.2)
			; //
			auto const
				p2 = x2
			; //
			auto const
				p3 =
					ZERO -
					(Y*0.2)
			; //
			show(p1);
			show(p2);
			show(p3);
			newline();
		}

		inline void
		CYCLOID_SEGMENT (
			vector2	const x1		,
			vector2	const x2		,
			size_t	const n = 100	,
			size_t	const s = 2
		) {
			TYPE_DATA const
				dt =
					1.0 /
					static_cast<TYPE_DATA>(n)
			; //
			auto const
				line =
					x2 - x1
			; //
			TYPE_DATA const
				radius =
					line.pt() /
					static_cast<TYPE_DATA>(s)
			; //
			auto const
				P =
					perp(line.dir())*
					radius
			; //

			auto const begin = x1 + P ;
			auto const end = x2 + P ;

			auto const X_vec =
				(P * static_cast<TYPE_DATA>(-1)).dir()
			; //
			auto const Y_vec =
				line.dir()
			; //
			for (
				size_t i=0;
				i<=n;
				i++
			) {
				TYPE_DATA const
					t =
						i * dt
				; //
				auto const
					angle =
						t * PI2 ()
				; //
				auto const vec =
					(X_vec*radius*std::cos(angle)) +
					(Y_vec*radius*std::sin(angle))
				; //
				auto center =
					( begin * (1.0 - t) ) +
					( end * t )
				;
				show(center+vec);
			}
		}
		inline void
		CYCLOID_LINE (
			vector2	const x1 ,
			vector2	const x2 ,
			size_t	const n = 3
		) {
			vector2 const mid =
				(x1 + x2) /
				static_cast<TYPE_DATA>(2)
			; //
			if (n>1) {
				CYCLOID_LINE(x1,mid,n-1);
				CYCLOID_LINE(mid,x2,n-1);
			} else if (n==1) {
				CYCLOID_SEGMENT(x1,x2,100);
			}
		}

		inline void
		FERMION (
			vector2 const x1 ,
			vector2 const x2
		) {
			auto const p1 =
				( x1 * 0.55 ) +
				( x2 * 0.45 )
			; //
			auto const p2 =
				( x2 * 0.55 ) +
				( x1 * 0.45 )
			; //
			LINE	(x1,x2) ;
			ARROW	(p1,p2) ;
		}

		inline void
		COMPLEX_SCALAR (
			vector2 const x1 ,
			vector2 const x2
		) {
			auto const p1 =
				( x1 * 0.55 ) +
				( x2 * 0.45 )
			; //
			auto const p2 =
				( x2 * 0.55 ) +
				( x1 * 0.45 )
			; //
			DASHED_LINE	(x1,x2) ;
			ARROW		(p1,p2) ;
		}

		inline void
		VECTOR (
			vector2 const x1 ,
			vector2 const x2
		) {
			auto const p1 =
				( x1 * 0.6 ) +
				( x2 * 0.4 )
			; //
			auto const p2 =
				( x2 * 0.6 ) +
				( x1 * 0.4 )
			; //
			WAVY_LINE	(x1,x2)	;
			newline		()		;
		}


		FeynDrawer (
			std::string _filename
		) : filename(_filename) {
			f =
				fopen (
					&(filename[0])	,
					"w"
				)
			; //
		}

		~FeynDrawer()
		{ fclose(f); }

	private:

		std::string		filename	;
		FILE		*	f			;

	} ;

}
