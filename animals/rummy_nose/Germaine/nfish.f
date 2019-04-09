	PROGRAM nfishing
C Model for N fish in circular arenas.
C Segmented trajectories (hemigrammus fish --rednose)
C
C  gfortran -O2 nfish.f fishfunc.f -lpgplot -o nfish
C
C Input files:
C	- fishfunc.f: contains the interaction functions
C
C Output files:
C	- path.dat: **THE output FILE** with the trajectories of all the fish
C		    in format "New Times Clément". Fish 1 is the kicking fish.
C	- path-pre.dat: previous to path.dat: contains the same information but
C			in another format (more civilized in my opinion...)
C	- 03.fA-analytic.dat: to represent the analytic expressions from fishfunc.f
C	- 03.*** same
C	- dens-***: data to represent PDF distributions of ln, taun and vn
C		    the characteristic parameters of the kick length and duration
C	- fish1.dat: trajectory of fish_1 (first columns) and of the other N-1 fish
C		     when the kicking fish is precisely fish_1.
C Questions, comments & suggestions: escobedor@gmail.com
C
	PARAMETER (Nfish=5,NDIM=2*Nfish, NBig=100000, NEmax=60)
	CHARACTER*9 filename
	REAL*8 U(NDIM),V(NDIM),F(NDIM),x(Nfish),y(Nfish)
	REAL*8 Unew(NDIM),Uold(NDIM),NormV(Nfish)
	REAL*8 Vx(Nfish),Vy(Nfish)
	REAL*8 fWall, gWall, fgw(Nfish), A, Deltat
	REAL*8 Domain, R1, R2, R3, RArena
	REAL*8 Phi(Nfish), d12(Nfish), DPhi(Nfish),Phinew,sDPhi
	REAL*8 Phi12(Nfish), psi12(Nfish), PDF(NBig)
	REAL*8 aux1(Nfish), aux2(Nfish)

	REAL*8 dij(Nfish,Nfish), psiij(Nfish,Nfish), Phiij(Nfish,Nfish)
	REAL*8 Thetaij(Nfish,Nfish),dd(Nfish),infl(Nfish,Nfish)
	INTEGER indice(Nfish),ppv(Nfish,Nfish),vip(Nfish,Nfish)
	INTEGER numbkicks(NEmax,Nfish) !number of kicks of each fish per event
	INTEGER Tnumbkicks(NEmax) ! Total number of kicks per event
	INTEGER Fnumbkicks(NEmax) ! Total number of kicks of each fish
C
	REAL*8 Theta(Nfish),ThetaW(Nfish), r(Nfish), rw(Nfish)
	REAL PGarrowX(Nfish),  PGarrowXOLD(Nfish)
	REAL PGarrowY(Nfish),  PGarrowYOLD(Nfish)
	REAL PGarrowVx(Nfish), PGarrowVxOLD(Nfish), Lx,Ly,lc,Bl
	REAL PGarrowVy(Nfish), PGarrowVyOLD(Nfish)
	REAL PGarrowFWx(Nfish),PGarrowFWxOLD(Nfish)  ! in preparation
	REAL PGarrowFWy(Nfish),PGarrowFWyOLD(Nfish)  !   for PGPLOT
	REAL PGarrowFHx(Nfish),PGarrowFHxOLD(Nfish)  !    (movies)
	REAL PGarrowFHy(Nfish),PGarrowFHyOLD(Nfish)
	REAL*8 Time(Nfish), Tiempo,TiempoOLD ! Tiempo = time in Spanish
	REAL*8 Bx(100),By(100) !Domain corners for PGPLOT
	REAL*8 PI, aa,bb, q,q1, q2,q3,q4,q5,q6, qx,qy
	REAL*8 c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c22,c33,c44
C
	REAL*8 Repu(Nfish),fRepu,gRepu,hRepu, DPhiRepu
	REAL*8  Ali(Nfish),fAli,gAli,hAli,    DPhiAli
	REAL*8 DPhiRand, DPhiWall, alpha, gammaRand, gammaWall
	REAL*8 vn(Nfish),ln(Nfish),tau0,taun(Nfish),beta(Nfish)
	REAL*8 g1,g2,rando(6) ! vector of random number (uniform)
C
	INTEGER nbox(NBig), is(Nfish), randi(Nfish)
	REAL*8 xx(NBig),yy(NBig),vv(NBig), Vmin,Vmax,Vmean
	REAL*8 xmin,xmax,xbox(NBig),qbox(NBig),xbmed,xm
	REAL*8 xpez(NEmax,Nfish,NBig),ypez(NEmax,Nfish,NBig)
	REAL*8 tpez(NEmax,NBig),tkicker
	
	REAL*8 BaryX(NEmax,NBig),BaryY(NEmax,NBig)
	REAL*8 VbaryX(NEmax,NBig),VbaryY(NEmax,NBig)
	REAL*8 BaryPhi(NEmax,NBig), BaryThetaW(NEmax,NBig)
	REAL BarrowX,BarrowXold, BarrowY,BarrowYold
C
	PI = DACOS(-1.D0)
	CALL SRAND(-641238561)
C
C -- Representation ---------------------------------------------------
	iREPR = 0 ! no dynamic representation, simply produce the data
	iREPR = 1 ! Dynamical evolution of points/circles using PGPLOT
	sizeAnt = 0.01 !size of the circle representing a fish --
					! ...comes from a code for ants
C -- Number of events -- New Times Clément format ---------------------
	Nevent = 10
	Deltat = 1.D-2 ! Universal time step for movies (if necessary)
	IF(Nevent.GT.NEmax) THEN
	  WRITE(*,*) 'Too much events; type Ctrl C'
	  READ(*,*) q
	ENDIF
	Nsec = 8000 ! Approx. 3/2 the final number of seconds per event
	ich = 1! representation each ''ich'' iterations
	ichich = 10000! representation each ''ichich'' iterations
	ichch = 1000*ich! representation each ''ichch'' iterations
C ---------------------------------------------------------------------
C Diameter of the ARENA:
	RArena = 0.25D0 !(meters)
	Domain = RArena*2.2D0 !(meters) for PGPLOT
C Rectangle in which the arena is inscribed (for PGPLOT):
	Bx(1) = - 0.5D0*Domain
	By(1) = - 0.5D0*Domain
	Bx(2) = Bx(1) + Domain
	By(2) = By(1)
	Bx(3) = Bx(2)
	By(3) = By(1) + Domain
	Bx(4) = Bx(1)
	By(4) = By(3)
	Bx(5) = Bx(1) ! closed polygonal
	By(5) = By(1)
C
	IF(iREPR.EQ.1.OR.iREPR.EQ.2) THEN ! call PGPLOT
	  CALL PGBEGIN(0,'/XWINDOW',1,1)   !output: xterm
	  CALL PGSCF(2)  ! 2 = roman font
	  CALL PGSCH(2.)! font size
	  CALL PGENV(REAL(Bx(1)),REAL(Bx(2))
     &		    ,REAL(By(1)),REAL(By(4)),1,1)
	  CALL PGSCH(1.)! font size
C Additional colors for several fish:
	  DO ink = 15,100 !add colors to indexes greater than 15
	    CG = (ink/100.)**0.3
	    CALL PGSCR (1+ink, 0.,CG, 1.)
	  ENDDO
C Specified colors: (les gouts et les couleurs...)
	  ink = 13
	  CALL PGSCR (ink, 255./255.,000./255.,204./255.)
	  ink = 16
	  CALL PGSCR (ink, 102./255.,000./255.,204./255.)
	  ink = 17
	  CALL PGSCR (ink, 102./255.,124./255.,000./255.)
	  ink = 18
	  CALL PGSCR (ink, 000./255.,204./255.,204./255.)
	  ink = 19
	  CALL PGSCR (ink, 204./255.,204./255.,000./255.)
	  ink = 20
	  CALL PGSCR (ink, 204./255.,000./255.,000./255.)
	  ink = 21
	  CALL PGSCR (ink, 204./255.,000./255.,204./255.)
	  ink = 22
	  CALL PGSCR (ink, 124./255.,124./255.,204./255.)
	  ink = 23
	  CALL PGSCR (ink, 124./255.,000./255.,124./255.)
	  ink = 24
	  CALL PGSCR (ink, 124./255.,000./255.,204./255.)
	ENDIF
C Parameters
C  ************** BEWARE: gammaWall is given in fishfunc.f
	BL = 0.03D0 !meters
	gammaRand = 0.35D0 !(rad) for 1 fish
	gammaRand = 0.45D0 !(rad) for 2 fish
c	gammaRand = 0.5D0 !(rad) for 5 fish
C	
	gammaWall = fWall(0.D0,RArena) !given in fishfunc.f
	alpha = 2.D0/3.D0 ! for 1 or 2 fish
c	tau0 = 0.8D0 !(s)  <--- defined in STEPER
	Nreject = 0
	lc = BL
	Vmean = 0.D0
	Nkick = 0 ! the first one is not a kick :-)
	Thetaij = 0.D0 !vector
	psiij = 0.D0 !vector
	Phiij = 0.D0 !vector
	numbkicks = 0 !vector
C
	DO i=1,Nfish
	  OPEN(100+i,file='fish'//CHAR(i+48)//'.dat') !1 file per fish
	ENDDO
C
C *********************************************************************
C *********************************************************************
C Mean values and PDFs from which l_n, v_n and tau_n are sampled:
	q1=0.D0
	q2=0.D0
	q3=0.D0
	xx = 0.D0!vector
	yy = 0.D0!vector
	vv = 0.D0!vector
	DO i=1,10000
	  DO ir=1,6
	    rando(ir) = rand(0)
	  ENDDO  
	  CALL STEPER(ln(1),vn(1),taun(1),tau0,rando)
	  q1 = q1+ln(1)
	  q2 = q2+vn(1)
	  q3 = q3+taun(1)
	  xx(i) = ln(1)
	  yy(i) = vn(1)
	  vv(i) = taun(1)
	ENDDO
	i=i-1
	WRITE(*,'("   ln: ",F7.4," < ",F7.4," < ",F9.4)')
     &		minval(xx),q1/i,maxval(xx)
	WRITE(*,'("   vn: ",F7.4," < ",F7.4," < ",F9.4)')
     &		minval(yy),q2/i,maxval(yy)
	WRITE(*,'(" taun: ",F7.4," < ",F7.4," < ",F9.4)')
     &		minval(vv),q3/i,maxval(vv)
C
	Nb = 81
	xmin = 0.D0
	xmax = -1.D0
	CALL DENSITOR(i,xx,Nb,xbox,nbox,qbox, xmin,xmax)
	OPEN(3,file='dens-ln.dat')
	DO ib=1,Nb
          xbmed = 0.5D0*(xbox(ib)+xbox(ib+1))
          WRITE(3,989) ib, xbmed, qbox(ib)
	ENDDO
	CLOSE(3)
C
	Nb = 81
	xmin = 0.D0
	xmax = -1.D0
	CALL DENSITOR(i,yy,Nb,xbox,nbox,qbox, xmin,xmax)
	OPEN(3,file='dens-vn.dat')
	DO ib=1,Nb
          xbmed = 0.5D0*(xbox(ib)+xbox(ib+1))
          WRITE(3,989) ib, xbmed, qbox(ib)
	ENDDO
	CLOSE(3)
C
	Nb = 81
	xmin = 0.D0
	xmax = -1.D0
	CALL DENSITOR(i,vv,Nb,xbox,nbox,qbox, xmin,xmax)
	OPEN(3,file='dens-taun.dat')
	DO ib=1,Nb
          xbmed = 0.5D0*(xbox(ib)+xbox(ib+1))
          WRITE(3,989) ib, xbmed, qbox(ib)
	ENDDO
	CLOSE(3)
C
	Nb = 81
	xmin = 1.D0
	xmax = - 1.D0
	CALL DENSITOR(6000,PDF,Nb,xbox,nbox,qbox, xmin,xmax)
	OPEN(3,file='dens-deltaPhi.dat')
	DO ib=1,Nb
          xbmed = 0.5D0*(xbox(ib)+xbox(ib+1))
          WRITE(3,989) ib, xbmed, qbox(ib)
	ENDDO
	CLOSE(3)
C *********************************************************************
C *********************************************************************
C ***************************** ITERATIONS ****************************
C *********************************************************************
C *********************************************************************
	DO 10 ievent=1,Nevent
C Initial condition: (position, phi & kick; different for each event) 
C ------------------
	Time = 0.D0 ! vector; Kicking times of fish i, i=1,...,Nfish
	DO i=1,Nfish
	  U(i)       =  0.5D0*(rand(0)-0.5D0)*RArena
	  U(Nfish+i) = -0.5D0*(rand(0)-0.5D0)*RArena
	  Phi(i) = 2.D0*(rand(0)-0.5D0)*PI
	  IF(DABS(U(i)).GT.RArena) WRITE(*,*) 'cond. ini. out of arena'
	  DO ir=1,6
	    rando(ir) = rand(0) ! vector of 6 random numbers for STEPER
	  ENDDO  
	  CALL STEPER(ln(i),vn(i),taun(i),tau0,rando)
	ENDDO
C To have same initial condition (close to each other & pointing North)
	U = 0.D0!vector
	DO i=1,Nfish
	  U(i) = -(i - 1.D0 - Nfish/2)*lc
	  Phi(i) = i*2.D0*PI/(Nfish+1.D0)
	  U(Nfish+i) = -0.1D0
	ENDDO
c	Phi = PI/2.D0 !vector
C ---------------------------------------------------------------------
C ---------------- The Nsec iterations of this event: -----------------
C ---------------------------------------------------------------------
	DO it=1,Nsec
	  Nkick = Nkick + 1
c  1) Detect which fish "ikicker" is the next one to kick:
	  ikicker = 1
	  tkicker = Time(1) + taun(1)
	  DO i=2,Nfish
	    q = Time(i) + taun(i)
	    IF(q.LT.tkicker) THEN
	      ikicker = i
	      tkicker = q
	    ENDIF
	  ENDDO  
	  numbkicks(ievent,ikicker) = numbkicks(ievent,ikicker) + 1
c  2) Position of the kicking fish at the end of its kick:
	  Tiempo = tkicker
	  WRITE(*,9999,ADVANCE='NO')
     &		  CHAR(13),ievent,it,INT(Time(ikicker))
	  Time(ikicker) = Time(ikicker) + taun(ikicker)
	  ix = ikicker
	  iy = ix+Nfish ! more easy to read
	  Unew(ix) = U(ix) + ln(ix)*DCOS(Phi(ix))
	  Unew(iy) = U(iy) + ln(ix)*DSIN(Phi(ix))
C  3) Position of the N-1 other fish at the end of the kick of ikicker
C     [since taun(i) < taun(the others)]. 
C     --- This is required to calculate the new PHI of ikicker
	  DO io=1,Nfish !io stands for "iother"
	    IF(io.NE.ikicker) THEN 
	      ixo = io	! this is one of "the others"
	      iyo = io + Nfish
	      q1 = Time(ix) - Time(ixo)
	              q =    1.D0 - DEXP(-q1 /tau0)
	      beta(ixo) = q/(1.D0 - DEXP(-taun(ixo)/tau0))
	      Unew(ixo) = U(ixo) + ln(ixo)*beta(ixo)*DCOS(Phi(ixo))
	      Unew(iyo) = U(iyo) + ln(ixo)*beta(ixo)*DSIN(Phi(ixo))
	    ENDIF
	  ENDDO
C  4) Write data in the file of the kicking fish, in Ramon's format:
C     (one file per fish; join files at the end for data analysis)
	  nfich = 100 + ix ! filename, for example
	  WRITE(nfich,'(I5,1x,F9.4,100(2x,F6.3,1x,F6.3))')
     &		  ievent, Time(ix)
     &		, Unew(ix),Unew(ix+Nfish)
     &		,(Unew( j),Unew( j+Nfish), j=1,ix-1)
     &		,(Unew( j),Unew( j+Nfish), j=ix+1,Nfish)
C
C  5) Prepare the new kick of fish i, i.e., calculate DPhi:
C - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
C Fish i has moved, the others too, but the others are in their gliding
C phase of their own kick. When i will make its next kick, it will be
C located at Unew, and its meighbors too, at Unew. The Unew of the
C others will be updated (corrected) in the time of the own kick of the
C other fish.
	  U(ix) = Unew(ix)
	  U(iy) = Unew(iy)
C Now we obtain the DPhi for fish ikicker for its next kick.
C - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
C Relative values dij, psiij, phiij:
	  i = ikicker ! to lighten notation... it is also ix :-)
	  DO j=1,Nfish
	    jx = j ! (jx,jy), j-th neighbor (not necessarily the j-th closest)
	    jy = j+Nfish
	    dij(i,j) = DSQRT( (Unew(ix) - Unew(jx))**2
     &		      	    + (Unew(iy) - Unew(jy))**2 )
	    Thetaij(i,j) = ATAN2(Unew(jy)-Unew(iy), Unew(jx)-Unew(ix))
C Perception of the other: psi_{ij} = theta_{ij} - \phi_i
	    psiij(i,j) = Thetaij(i,j) - Phi(i)
111	      IF(psiij(i,j).LT.-PI) psiij(i,j) = psiij(i,j) + 2.D0*PI
	      IF(psiij(i,j).GT. PI) psiij(i,j) = psiij(i,j) - 2.D0*PI
	      IF(DABS(psiij(i,j)).GT.PI) GOTO 111
C Relative orientation of j resp. to i: Phiij = Phi_j - Phi_i
	    Phiij(i,j) = Phi(j) - Phi(i)
112	      IF(Phiij(i,j).LT.-PI) Phiij(i,j) = Phiij(i,j) + 2.D0*PI
	      IF(Phiij(i,j).GT. PI) Phiij(i,j) = Phiij(i,j) - 2.D0*PI
	      IF(DABS(Phiij(i,j)).GT.PI) GOTO 112
	  ENDDO
C
C Neighbors of fish i, ordered by closeness to fish i:
C ppv(i,j) is the index of the j-th closest neighbor of fish i
	  DO j=1,Nfish
	    dd(j) = dij(i,j)
	  ENDDO
	  CALL PlusPrVois(Nfish,dd,indice)
	  DO j=1,Nfish-1
	    ppv(i,j) = indice(j+1) ! indice(1) is fish i
	  ENDDO
C	  
C Influence of Pj on Pi, from the analytic expressions:
	  DO j=1,Nfish
	      q1 = dij(i,j)
	      q2 = psiij(i,j)
	      q3 = Phiij(i,j)
	      q4 = fRepu(q1,RArena)*gRepu(q2)*hRepu(q3)
	      q5 = fAli( q1,RArena)*gAli( q2)*hAli( q3)
	      infl(i,j) = DABS(q4 + q5)
	      IF(i.EQ.j) infl(i,j) = 0.D0
	  ENDDO
C
C Voisins les plus influents: (vip: Voisin qui Influe le Plus)
C ---------------------------
C vip(i,j) is the index of the j-th most influent neighbor.
C It can happen that the maximum influence is zero; then, the
C neighbors are ordered according to closeness to focal fish
C (i.e., we use ppv).
	  DO j=1,Nfish
	    dd(j) = infl(i,j)
	  ENDDO
	  IF(sum(dd).GT.1.D-6) THEN
	    CALL VoisInflPlus(Nfish,dd,indice)
	    DO j=1,Nfish-1
	      vip(i,j) = indice(j)
	    ENDDO
	  ELSE ! la influencia es CERO
	    DO j=1,Nfish-1
	      vip(i,j) = ppv(i,j)
	    ENDDO
	  ENDIF
C
C Now do the step using the model:
C --> Data of position, orientation and alignment of fish i:
           r(i) = DSQRT(U(ix)**2 + U(iy)**2)
	  rw(i) = RArena - r(i)
	  Theta(i) = ATAN2(U(iy) ,U(ix))
	  ThetaW(i) = Phi(i) - Theta(i)
113	    IF(ThetaW(i).LT.-PI) ThetaW(i)=ThetaW(i)+2.D0*PI
	    IF(ThetaW(i).GT. PI) ThetaW(i)=ThetaW(i)-2.D0*PI
	    IF(DABS(ThetaW(i)).GT.PI) GOTO 113
C
C --> Sample the new values of "ln, vn, taun" for fish i:
114	  DO ir=1,6
	    rando(ir) = rand(0)
	  ENDDO  
	  CALL STEPER(ln(i),vn(i),taun(i),tau0,rando)	  
C
C --> Compute the new Phi = Phi + DPhi for fish i:
C   - Free will:
	  aa = 2.D0*PI * rand(0)
	  bb = DSQRT(-2.D0*DLOG(rand(0) + 1.D-16)) ! prevents log(0)
	  g1 = bb * DSIN(aa) ! Normal(0,1)
	   q = 1.D0*alpha * fWall(rw(i),RArena)/gammaWall
	  DPhiRand = gammaRand * (1.D0-q) * g1
C   - Interactions: Wall (if...), Attraction/Repulsion and Alignment 
	   fgw(i) = fWall(rw(i),RArena) * gWall(ThetaW(i))
	  Repu(i) = 0.D0
	   Ali(i) = 0.D0
	   randi = 0 !vector of integers
	   CALL RandVec(randi,Nfish) !fill randi with random indexes
	   
		kc = 4 	! *** NUMBER OF NEIGHBORS TAKEN INTO ACCOUNT ***
	  
	  DO j=1,kc	! kc = Nfish-1 means all the neighbors.
c	    jk = randi(j) ! j-th random index in randi
c	    IF(jk.EQ.i) jk = randi(Nfish) ! not to use focal fish as a neighbor
C
c	    jk = ppv(ikicker,j) ! j-th closest neighbor of fish i
C
	    jk = vip(ikicker,j) ! j-th strongest influencer neighbor of fish i
C
	    Repu(i) = Repu(i) + fRepu(dij(i,jk),RArena)
     &			     *gRepu(psiij(i,jk))*hRepu(Phiij(i,jk))
	     Ali(i) = Ali(i)  +  fAli(dij(i,jk),RArena)
     &			     * gAli(psiij(i,jk))* hAli(Phiij(i,jk))
	  ENDDO
C --> Then:
	  DPhiRand = 1.D0*DPhiRand
	  DPhiWall = 1.D0*fgw(i)
	  DPhiRepu = 1.D0*Repu(i)
	  DPhiAli  = 1.D0*Ali(i)
C
	  DPhi(i)  = DPhiRand + DPhiWall + DPhiRepu + DPhiAli
	  PDF((ievent-1)*Nsec+it) = DPhi(i)
C
	  Phi(i) = Phi(i) + 1.D0*DPhi(i)
115	  IF(Phi(i).LT.-PI) Phi(i) = Phi(i) + 2.D0*PI
	  IF(Phi(i).GT. PI) Phi(i) = Phi(i) - 2.D0*PI
	  IF(DABS(Phi(i)).GT.PI) GOTO 115
C --------------------------------------------.
C Rejection test: ONLY WHEN THERE IS A BORDER
	  qx = Unew(ix) + (ln(i)+lc)*DCOS(Phi(i))
	  qy = Unew(iy) + (ln(i)+lc)*DSIN(Phi(i))
	  IF(DSQRT(qx*qx + qy*qy).GT.RArena) THEN !se va a salir
	    ireject = ireject + 1
c	    IF(Nreject.GT.500) THEN
c	      WRITE(*,*) Nreject,'#rejects > 500; type 0 to cont.'
c	      READ(*,*) q
c	    ENDIF
	    GOTO 114 
	  ENDIF
cc	  WRITE(*,*) ievent,it,ite,Nreject
	  Nreject = Nreject + ireject
	  ireject = 0
C -- end rejection test ----------------------.
C
C Barycenter measures:
C --------------------
C Fish velocities:
	    DO i=1,Nfish
	      Vx(i) = (Unew(i)       - UOld(i)      )/taun(ikicker)
	      Vy(i) = (Unew(i+Nfish) - UOld(i+Nfish))/taun(ikicker)
	    ENDDO
C Coordinates and velocity of the Barycenter
	     BaryX(ievent,it) = 0.D0
	     BaryY(ievent,it) = 0.D0
	    VbaryX(ievent,it) = 0.D0
	    VbaryY(ievent,it) = 0.D0
	    DO i=1,Nfish
	       BaryX(ievent,it) =  BaryX(ievent,it) + Unew(i)
	       BaryY(ievent,it) =  BaryY(ievent,it) + Unew(i+Nfish)
	      VbaryX(ievent,it) = VbaryX(ievent,it) + Vx(i)
	      VbaryY(ievent,it) = VbaryY(ievent,it) + Vy(i)
	    ENDDO
	     BaryX(ievent,it) =  BaryX(ievent,it)/Nfish
	     BaryY(ievent,it) =  BaryY(ievent,it)/Nfish
	    VbaryX(ievent,it) = VbaryX(ievent,it)/Nfish
	    VbaryY(ievent,it) = VbaryY(ievent,it)/Nfish
C Heading and ThetaW of Bary:
	    BaryPhi(ievent,it) =
     &		    ATAN2(VbaryY(ievent,it),VbaryX(ievent,it))
	    q = BaryPhi(ievent,it) 
     &		    - ATAN2( BaryY(ievent,it), BaryX(ievent,it))
118	      IF(q.LT.-PI) q = q + 2.D0*PI
	      IF(q.GT. PI) q = q - 2.D0*PI
	      IF(DABS(q).GT.PI) GOTO 118	    
	    BaryThetaW(ievent,it) = q
C
C REPRESENTATION -- PGPLOT:
C -------------------------
	  IF(iREPR.GE.1) THEN
	    qs = 0.05 !length of the arrow for PGPLOT
	    DO i=1,Nfish
	      rx = REAL(Unew(i)       - Uold(i))
	      ry = REAL(Unew(i+Nfish) - Uold(i+Nfish))
	      rV = SQRT(rx**2 + ry**2)
	      PGarrowX(i) = REAL(Unew(i))       + qs*rx/rV
	      PGarrowY(i) = REAL(Unew(i+Nfish)) + qs*ry/rV
	    ENDDO
	    q = BaryPhi(ievent,it)
	    BarrowX = REAL(BaryX(ievent,it) + qs*DCOS(q))
	    BarrowY = REAL(BaryY(ievent,it) + qs*DSIN(q))
C	    
	    CALL PGdynamic(Nfish,Tiempo,TiempoOld,Unew,Uold
     &		,REAL(Bx),REAL(By),sizeAnt,RArena
     &		,PGarrowX,PGarrowY,PGarrowXOLD,PGarrowYOLD
     &		,BaryX,BaryY,NEmax,NBig,ievent,it
     &		,BarrowX,BarrowY,BarrowXold,BarrowYold)
	    TiempoOld = Tiempo
	    Uold = Unew
	    PGarrowXOLD = PGarrowX
	    PGarrowYOLD = PGarrowY
	    BarrowXold = BarrowX
	    BarrowYold = BarrowY
	  ENDIF
	ENDDO !--> next iteration (next kick in this event)
10	CONTINUE !--> next event
C
C *********************************************************************
C Close datafiles of each fish:
	DO i=1,Nfish
	  nfich = 100+i
	  CLOSE(nfich)
	ENDDO  
	CALL PGEND
C
C *********************************************************************
C *********************************************************************

C 			     Postprocessing
C
C *********************************************************************
C *********************************************************************
	WRITE(*,*)
	WRITE(*,'(" 1 reject in ",F6.2," % of the kicks")')
     &			,100.*REAL(Nreject)/Nkick
	Fnumbkicks = 0
	DO i=1,Nfish
	  DO iev=1,Nevent
	    Fnumbkicks(i) = Fnumbkicks(i) + numbkicks(iev,i)
	  ENDDO
	ENDDO
	WRITE(*,'(" #kicks of each fish: ",5(I6,1x))')
     &			(Fnumbkicks(i), i=1,Nfish)
	WRITE(*,'(A20,3I8)') 'Total kicks & check:'
     &			,sum(numbkicks),Nkick
C
	DO i=1,Nfish
	  filename='fish'//CHAR(i+48)//'.dat'
	  OPEN(81,FILE=filename)
	  nl = 0
	  DO il=1,NBig
	    READ(81,*,end=77) Nev1
	    nl = nl + 1
	  ENDDO
77	  CLOSE(81)
	  IF(nl.NE.Fnumbkicks(i)) THEN
	    WRITE(*,*) 'Bad lines number in file',filename
	    WRITE(*,*) 'Type Ctrl + C to stop'
	    READ(*,*) q
	  ENDIF
	ENDDO
C
C ---------------------------------------------------------------------
C Now data must be recorded in a special format, "New Times Clément".
C 1) Build path.dat, which contains the kicks alone.
C       The kicking fish is iped = 1, the other may or may not be kicking.
C 2) Build path+kicks.dat, with the full trayectories of both fish, in-
C       cluding times between instants where kicks have been defined.
C ---------------------------------------------------------------------
C 1i) Joint both files (one per fish) in one file. Note that the counter
C     of events of the second file should start at value the counter of
C     the first file finishes, plus one.
C

C Join the Nfish files fishi.dat in path-pre:
	OPEN(2,FILE='path-pre.dat')
	DO i=1,Nfish
	  filename='fish'//CHAR(i+48)//'.dat'
	  OPEN(81,FILE=filename)
	  DO it=1,Fnumbkicks(i)
	    READ(81,*) i1,qt,(xpez(1,j,it),ypez(1,j,it),j=1,Nfish)
	    WRITE(2,'(I5,1x,F9.4,100(2x,F6.3,1x,F6.3))')
     &	    (i-1)*Nevent+i1, qt, (xpez(1,j,it),ypez(1,j,it),j=1,Nfish)
	  ENDDO
	  CLOSE(81)
	ENDDO
	CLOSE(2)
C
C Rewrite path-pre.dat in New Times Clément --> path.dat
	OPEN(81,FILE='path-pre.dat')
	nl=0
	iev = 0
	it = 0
	tpez = 0.D0 !vector
	DO il=1,Nkick
	  READ(81,*) i1,qt, (aux1(j),aux2(j), j=1,Nfish)
	  nl = nl+1
	  IF(i1.NE.iev) it = 0
	  iev = i1
	  it = it+1
	  tpez(iev,it) = qt
	  DO j=1,Nfish
	    xpez(iev,j,it) = aux1(j)
	    ypez(iev,j,it) = aux2(j)
	  ENDDO
	ENDDO
	CLOSE(81)
C
	WRITE(*,*) 'writing path.dat...'
	OPEN(80,FILE='path.dat')
	DO iev=1,Nfish*Nevent
	  it = 0
	  DO 4 i=1,Nfish
421	    it = it + 1
	    WRITE(80,'(I5,1x,I2,1x,F9.4,2x,F6.3,1x,F6.3)')
     &	    iev,i,tpez(iev,it),xpez(iev,i,it),ypez(iev,i,it)
	    IF(tpez(iev,it+1).LT.tpez(iev,it)) THEN
	      it = 0
	      GOTO 4
	    ENDIF  
	    GOTO 421
4	  ENDDO
	ENDDO
	CLOSE(80)
C
C ************************************************************* FIN ***
C Write the interaction functions:
	OPEN(700,file='03.fw-analytic.dat')
	OPEN(701,file='03.gw-analytic.dat')
	OPEN(702,file='03.fR-analytic.dat')
	OPEN(703,file='03.gR-analytic.dat')
	OPEN(704,file='03.hR-analytic.dat')
	OPEN(705,file='03.fA-analytic.dat')
	OPEN(706,file='03.gA-analytic.dat')
	OPEN(707,file='03.hA-analytic.dat')
C
	DO ir=1,101 !Nr
	  xm = (ir-0.5D0)*(Rmax-Rmin)/101.D0
	  WRITE(700,*) xm,fWall(xm,RArena)
	ENDDO
	DO ith=1,101 !Nth
	  xm = (ith-0.5D0)*2.D0*PI/101.D0 - PI
	  WRITE(701,*) xm,gWall(xm)
	ENDDO
C
	DO id=1,101 !Nd
	  xm = (id-0.5D0)*2.D0*RArena/101.D0
	  WRITE(702,*) xm,fRepu(xm,RArena)
	  WRITE(705,*) xm,fAli(xm,RArena)
	ENDDO
	DO ipsi=1,101 !Npsi
	  xm = (ipsi-0.5D0)*2.D0*PI/101.D0 - PI
	  WRITE(703,*) xm,gRepu(xm)
	  WRITE(706,*) xm,gAli(xm)
	ENDDO
	DO iphi=1,101 !Nphi
	  xm = (iphi-0.5D0)*2.D0*PI/101.D0 - PI
	  WRITE(704,*) xm,hRepu(xm)
	  WRITE(707,*) xm,hAli(xm)
	ENDDO
C
	CLOSE(700)
	CLOSE(701)
	CLOSE(702)
	CLOSE(703)
	CLOSE(704)
	CLOSE(705)
	CLOSE(706)
	CLOSE(707)
C
997	FORMAT(F10.2,2X,100(F12.4,1X))
989	FORMAT(I6,2x,F8.4,10(1x,F9.4))
9997	FORMAT(I5,1x,I3,1x,F10.4,2X,100(F11.6,1X))
9998	FORMAT(I3,1x,F9.4,1x,500(2x,F10.3,1x,F10.3))
c9999	FORMAT(A1,1x,I3,1x,I7)
9999	FORMAT(A1,1x,5I7)
	END
C
C *********************************************************************
C *********************************************************************
C *************** SUBROUTINES: MODES OF REPRESENTATION ****************
C *********************************************************************
C *********************************************************************
	SUBROUTINE PGdynamic(Nfish,Time,TimeOLD,U,Uold,Bx,By
     &		,sizeAnt,RArena
     &		,PGarrowX,PGarrowY,PGarrowXOLD,PGarrowYOLD
     &		,BaryX,BaryY,NEmax,NBig, iev,it
     &		,BarrowX,BarrowY,BarrowXold,BarrowYold)
	REAL*8 Time,TimeOLD,U(2*Nfish),Uold(2*Nfish),RArena
	REAL Bx(5),By(5),sizeAnt, qx,qy,qV
	REAL PGarrowX(Nfish),PGarrowXOLD(Nfish)
	REAL PGarrowY(Nfish),PGarrowYOLD(Nfish)
	REAL BarrowX,BarrowY, BarrowXold,BarrowYold
	REAL*8 BaryX(NEmax,NBig),BaryY(NEmax,NBig)
	CHARACTER*8 TEXT1
	CHARACTER*1 TEXT2
C
	Ncola = 10 ! cola = tail in Spanish
	NcolaL = 20 ! long tail
	Deltat = 1./2.
C
	CALL slip(.1) ! pause a portion of time
	  
	iOldTime = INT(TimeOLD * 100)
	iTime =    INT(Time * 100)
C
C Delete old time: (i.e., write it in black)
	CALL PGSLW(1) ! line width
	CALL PGSCI(0) ! ink = black
	CALL PGSCH(2.)! font size
	CALL PGNUMB(iOldTime,-2,1,TEXT1,nc)
	CALL PGPTXT (0.65*Bx(1),1.1*By(3)
     &				,0.,0.,'Time (s): '//TEXT1)
C Write new time:
	CALL PGSLW(1) ! line width
	CALL PGSCI(1) ! ink = white
	CALL PGSCH(2.)! font size
	CALL PGNUMB(iTime,-2,1,TEXT1,nc)
	CALL PGPTXT (0.65*Bx(1),1.1*By(3)
     &				,0.,0.,'Time (s): '//TEXT1)
C Delete the old barycenter:
	CALL PGSCI(0) ! ink=black
	DO iit=it-NcolaL-NcolaL,it-NcolaL
	  CALL PGCIRC(REAL(BaryX(iev,iit)),REAL(BaryY(iev,iit))
     &			  ,sizeAnt/2.)
	ENDDO
C Delete the old arrows
	CALL PGSCH(1.)
	CALL PGSLW(10) ! line width
	DO i=1,Nfish
	  CALL PGSCI(0) ! ink=black
	  CALL PGARRO(REAL(UOld(i)),REAL(UOld(i+Nfish))
     &		     ,PGarrowXOld(i),PGarrowYOld(i))
	ENDDO
	CALL PGARRO(REAL(BaryX(iev,it-1)),REAL(BaryY(iev,it-1))
     &		     ,BarrowXold,BarrowYold)
C Draw the arena:
	CALL PGSCI(2) ! ink = red
	CALL PGSLW(1) ! line width
	CALL PGSFS(2) !unFilled
	CALL PGCIRC(0.,0.,REAL(RArena))
C The new colored circle:
C   the old position in slight color
	CALL PGSLW(10) ! line width
	CALL PGSCH(2.)
	CALL PGSFS(1) !Filled
	DO i=1,Nfish
	  CALL PGSCI(14) ! ink=gray
	  CALL PGSCI(0) ! ink=black
	  CALL PGSFS(1) !Filled
	  CALL PGCIRC(REAL(Uold(i)),REAL(Uold(i+Nfish)),sizeAnt)
	ENDDO
C   the new one in strong color
	CALL PGSCI(15)	! ink=gray
	DO iit=it-NcolaL,it
	  CALL PGCIRC(REAL(BaryX(iev,iit)),REAL(BaryY(iev,iit))
     &			  ,sizeAnt/2.)
	ENDDO
	  
	CALL PGSCH(2.)
	CALL PGSFS(1) !Filled
	DO i=1,Nfish
	  CALL PGSCI(1+i) ! ink
	  CALL PGSFS(1) !Filled
	  CALL PGCIRC(REAL(U(i)),REAL(U(i+Nfish)),sizeAnt)
	ENDDO
C   the arrows
	CALL PGSCH(1.)
	CALL PGSLW(10) !grosor de linea
	DO i=1,Nfish
	  CALL PGSCI(1+i) ! ink
	  CALL PGARRO(REAL(U(i)),REAL(U(i+Nfish))
     &		     ,PGarrowX(i),PGarrowY(i))
	ENDDO
	CALL PGSLW(6) !grosor de linea
	CALL PGSCI(15) ! ink
	CALL PGARRO(REAL(BaryX(iev,it)),REAL(BaryY(iev,it))
     &		     ,BarrowX,BarrowY)
C
	RETURN
	END
C
C *********************************************************************
	subroutine slip(etim)
C etim IS A FRACTION OF SECONDS TO WAIT
	real ttim,etim
	TTIM = SECNDS(0.0)
	DO WHILE(SECNDS(TTIM).LE.ETIM)
	AA = AA
	END DO
	return
	end 
C
C *********************************************************************
	SUBROUTINE DENSITOR(N,X,Nb,xbox,nbox, qbox, xmin,xmax)
C Calcula la funcion de densidad de probabilidad de una magnitud X
C que toma N valores entre xmin y xmax (dados), en una dimension.
C Si xmin > xmax, entonces los calcula el programa. 
C   Nb: nº de boxes de valores de X.
C   ib: indice del box
C   xbox(1:Nb): abscisa del extremo inferior del ib-esimo box.
C    --> xbox(Nb+1) es el extremo superior del Nb-esimo box.
C   nbox(ib): numero de valores que han caido en el ib-esimo box
C   qbox(ib): densidad de probabilidad de X por u. de longitud de box.
C   L: longitud del intervalo donde X toma sus valores.
C   Lbox: longitud de un box.
C
C La integral de la funcion de densidad de prob. debe ser 1, es decir:
C		\int_xmin^xmax q(X)dX = 1,
C peeeero esta integral es una suma (integral de Riemann):
C 		\int_xmin^xmax q(X)dX = \sum_{ib=1}^{Nb} q(ib) Lbox
C donde Lbox aparece porque en la integral de Riemann es el valor que
C se suma es [el valor de la funcion]x[la longitud del intervalo],
C y el valor es q(ib) = nbox(ib) / (Lbox x N).
C OJO: Esto es valido en una dimension; para obtener la densidad radial
C hay que hacer cuentas porque lo que tiene que dar 1 es:
C   \int_{-pi}^{pi} \int_0^R rho(r,theta) rdr dtheta; ojo al jacobiano!
C   
	REAL*8 X(N), xbox(Nb+1),qbox(Nb+1), q, L, Lbox, PI
	REAL*8 xmin,xmax
	INTEGER nbox(Nb)
	PI = DACOS(-1.D0)
C Construccion de los boxes:
	IF(xmax.LE.xmin) THEN !truco: asi los busca él.
	  xmin =  1.D8
	  xmax = -1.D8
	  DO i=1,N
	    IF(X(i).GT.xmax) xmax = X(i)
	    IF(X(i).LT.xmin) xmin = X(i)
	  ENDDO
	ENDIF
	L = xmax - xmin
	Lbox = L/Nb
C
	xbox = 0.D0 !vector
	DO ib=1,Nb+1
	  xbox(ib) = xmin + (ib-1.D0)*Lbox
	ENDDO
C Rellenado de los boxes:
	nbox = 0 !vector
	DO 1 i=1,N ! En que caja ib está X(i)?
C -- primero, comprobar que el dato esta en [xmin,xmax]:
	  IF(X(i).LT.xmin.OR.X(i).GT.xmax) GOTO 1
C -- luego, ver si es exactamente
C -- el extremo derecho del ultimo box:
	  ib = INT((X(i)-xmin)*Nb/L)+1
	  IF(X(i).LE.xmin.OR.ib.LT.1) ib = 1
	  IF(X(i).GE.xmax.OR.ib.GT.Nb) ib = Nb
	  nbox(ib) = nbox(ib) + 1
1	CONTINUE
C Calculo del numero de datos total ntot
        ntot = 0
	DO ib=1,Nb
	  ntot = ntot + nbox(ib)
        ENDDO
C Calculo de la densidad por unidad de longitud de box:        
cccccc        qbox = REAL(nbox)/(ntot * Lbox)

        qbox = REAL(nbox)/(N * Lbox)
c
c	WRITE(*,'(A10,1x,I6,1X,I6,2X,A5,1X,F8.4)')
c     &		'DENSITOR: ',N,ntot,"Lbox:",Lbox
	RETURN
	END
C
C ---------------------------------------------------------------------
	SUBROUTINE PlusPrVois(N,dd,i)
C Plus proche voisin = nearest neighbor in French
C Uses dd and d to protect dd (remains unchanged)
C i is the vector of the ordered indexes of the neighbors
C i=1 is the focal fish itself, i=2 is its nearest neighbor,...
	REAL*8 d(N),dd(N),q
	INTEGER i(N)
	d = dd !vector
	DO k=1,N
	  i(k) = k
	ENDDO
10	iflip = 0
	DO k=1,N-1
	  IF(d(k).GT.d(k+1)) THEN
	    iflip = 1
	    q = d(k)
	    d(k) = d(k+1)
	    d(k+1) = q
	    iq = i(k)
	    i(k) = i(k+1)
	    i(k+1) = iq
	  ENDIF
	ENDDO
	IF (iflip.EQ.1) GOTO 10
	RETURN
	END
C *********************************************************************
	SUBROUTINE VoisInflPlus(N,dd,i) ! dd = influence
C Note that the focal fish can appear as its own "influential neighbor"
C when e.g. the second most influent has influence zero and so on...
	REAL*8 d(N),dd(N),q
	INTEGER i(N)
	d = dd !vector
	DO k=1,N
	  i(k) = k
	ENDDO
10	iflip = 0
	DO k=1,N-1
	  IF(d(k).LT.d(k+1)) THEN
	    iflip = 1
	    q = d(k)
	    d(k) = d(k+1)
	    d(k+1) = q
	    iq = i(k)
	    i(k) = i(k+1)
	    i(k+1) = iq
	  ENDIF
	ENDDO
	IF (iflip.EQ.1) GOTO 10
	RETURN
	END
C *********************************************************************
	SUBROUTINE RandVec(rv,N)
C Creates a vector of size N with a random order of indexes:
C e.g. for N=4: ov(1)=3, ov(2)=1, ov(3)=4 and ov(4)=2.
C Requires CALL SRAND(-92541...) in main.f
	INTEGER rv(N)
	rv = 0 !vector
	DO i=1,N
1	  k = INT(rand(0)*N) + 1
	  DO j=1,i-1
	    IF(k.EQ.rv(j)) GOTO 1
	  ENDDO
	  rv(i) = k
	ENDDO
	RETURN
	END
C --------------------------------------------
	SUBROUTINE STEPER(ln,vn,taun,tau0,rando)
C Calculates the data for the micro-steps between kicks --gliding phase
C without changing orientation:
C      vn: peak speed (m/s)
C      ln: length of the kick (m)
C    taun: duration of the kick (s)
C
C Maxwell-Boltzmann distributed with mean a:
C x = a * DSQRT(2.D0/PI) * DSQRT(-2.D0*DLOG(rand(0) + 1.D-16))
C
	REAL*8 ln,vn,taun,tau0, vel,length,time
	REAL*8 PI,bb,rando(6), mu,sig,x,Fx ! vector de numero aleatorios
	PI = DACOS(-1.D0)
	  tau0 = 0.8D0 ! <-- from the paper
c mean values:
	   vel = 0.14D0 ! <-- from
	length = 0.07D0 ! <-- the
	  time = 0.5D0  ! <-- paper
C ----------------------
	
	bb = DSQRT(-2.D0*DLOG(rando(2) + 1.D-16)) ! prevents log(0)
	vn = vel*DSQRT(2.D0/PI)*bb
	    
	bb = DSQRT(-2.D0*DLOG(rando(4) + 1.D-16)) ! prevents log(0)
	ln = length*DSQRT(2.D0/PI)*bb
	
cc	x = 0.D0
cc	ln = length
cc	do i=0,20000
cc	  x = x + 0.0001D0
cc	  Fx = 0.05D0 + 0.95D0*ERF(8.95D0*x)
cc	  IF(Fx.GT.rando(1)) THEN
cc	    ln = x
cc	    EXIT
cc	  ENDIF 
cc	  if(i.ge.20000) THEN
cc	    write(*,*) 'Impossible inversion in Steper:erf',fx,rando(1)
cc	    write(*,*) 'Type Ctrl C'
cc	    read(*,*) q
cc	  endif
cc	enddo  
	    
	bb = DSQRT(-2.D0*DLOG(rando(6) + 1.D-16)) ! evita log(0)
	taun = time*DSQRT(2.D0/PI)*bb
C
C From two of them, I deduce the third according to the formula:	    
c	vn = ln/(tau0*(1.D0-DEXP(-taun/tau0)))
	ln = vn*tau0*(1.D0-DEXP(-taun/tau0))
	RETURN
	END
C
