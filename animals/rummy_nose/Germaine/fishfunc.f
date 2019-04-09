C Interaction functions for fish -- Wall, attraction and alignment
C *********************************************************************
C   Interaction with the wall: fw, gw
C *********************************************************************
	FUNCTION fWall(rw,RArena)
	REAL*8   fWall,rw,RArena,x,r0, FF, gammaWall,l
	x = DMAX1(0.D0,rw)
	gammaWall = 0.12D0 !(rad) <-- from the paper for N = 1 fish
	gammaWall = 0.15D0 !(rad) <-- from the paper for N > 1 fish
	l = 0.06D0	   !(m) <---- from the paper for N = 1,2 fish
	fWall = gammaWall*DEXP(-(x/l)**2) ! <-- from the paper
	RETURN
	END
C ---------------------------
	FUNCTION gWall(ThetaW) !Odd
	REAL*8   gWall,ThetaW,x, a0, a1, a2
	x = ThetaW
	a0 = 1.915651D0
	gWall = a0*DSIN(x)*(1.D0 + 0.7D0*DCOS(2.D0*x))  ! <-- from the paper
	RETURN
	END
C *********************************************************************
C   Attraction/repulsion: fR, gR, hR 
C	the functio is called repulsion but is the attraction...
C	this comes from a code for pedestrians.
C *********************************************************************
	FUNCTION fRepu(d,RArena)
	REAL*8   fRepu,d,RArena, a0, a1, a2
	a0 = 4.D0	! <-- from
	a1 = 0.03D0	! <-- the
	a2 = 0.2D0	! <-- paper
	fRepu = a0*(d-a1)/(1.D0+(d/a2)**2) ! <-- from the paper
	RETURN
	END
C ---------------------------
	FUNCTION gRepu(psi12) ! Odd
	REAL*8   gRepu,psi12,x, a0,a1,a2
	x = psi12
	gRepu = 1.395D0*DSIN(x)*(1.D0 - 0.33D0*DCOS(x)) ! <-- from the paper
	RETURN
	END
C ---------------------------
	FUNCTION hRepu(Phi12) ! Even
	REAL*8   hRepu,Phi12, x, a0, a1, a2
	x = Phi12
	hRepu = 0.93263D0*(1.D0 - 0.48D0*DCOS(x) - 0.31D0*DCOS(2.D0*x)) ! <-- from the paper
	! there is a mistake in the paper: the sign of 0.48 is "-".
	RETURN
	END
C *********************************************************************
C   Alignement: fA, gA, hA
C *********************************************************************
	FUNCTION fAli(d,RArena)
	REAL*8   fAli,d,RArena, a0, a1, a2
	a0 = 1.5D0	! <-- from
	a1 = 0.03D0	! <-- the
	a2 = 0.2D0	! <-- paper
	fAli = a0*(d+a1)*DEXP(-(d/a2)**2) ! <-- from the paper
	RETURN
	END
C ---------------------------
	FUNCTION gAli(psi12) ! Even
	REAL*8   gAli,psi12,x, a0, a1, a2
	x = psi12
	gAli = 0.90123D0*(1.D0 + 0.6D0*DCOS(x) - 0.32D0*DCOS(2.D0*x)) ! <-- from the paper
	RETURN
	END
C ----------------------
	FUNCTION hAli(Phi12) ! Odd
	REAL*8   hAli,Phi12,x, a0, a1, a2
	x = Phi12
	hAli = 1.6385D0*DSIN(x)*(1.D0 + 0.3D0*DCOS(2.D0*x)) ! <-- from the paper
	RETURN
	END
