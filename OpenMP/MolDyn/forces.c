
/*
 *  Compute forces and accumulate the virial and the potential
 */
  extern double epot, vir;

  void
  forces(int npart, double x[], double f[], double side, double rcoff){
    int i, j;
    double sideh, rcoffs;
    double xi,yi,zi,fxi,fyi,fzi,xx,yy,zz;
    double rd, rrd, rrd2, rrd3, rrd4, rrd6, rrd7, r148;
    double forcex, forcey, forcez;

    vir    = 0.0;
    epot   = 0.0;
    sideh  = 0.5*side;
    rcoffs = rcoff*rcoff;

    #pragma omp parallel for default(none) \
    shared(x,npart,side,sideh,rcoffs,f) \
    private(i,j,xi,yi,zi,fxi,fyi,fzi,xx,yy,zz,rd,rrd,rrd2,rrd3,rrd4,rrd6,rrd7,r148,forcex,forcey,forcez) \
    reduction(+:vir,epot)
    for (i=0; i<npart*3; i+=3) {
      xi  = x[i];
      yi  = x[i+1];
      zi  = x[i+2];
      fxi = 0.0;
      fyi = 0.0;
      fzi = 0.0;

      for (j=i+3; j<npart*3; j+=3) {
        xx = xi-x[j];
        yy = yi-x[j+1];
        zz = zi-x[j+2];
        if (xx<-sideh) xx += side;
        if (xx> sideh) xx -= side;
        if (yy<-sideh) yy += side;
        if (yy> sideh) yy -= side;
        if (zz<-sideh) zz += side;
        if (zz> sideh) zz -= side;
        rd = xx*xx+yy*yy+zz*zz;

        if (rd<=rcoffs) {
          rrd      = 1.0/rd;
          rrd2     = rrd*rrd;
          rrd3     = rrd2*rrd;
          rrd4     = rrd2*rrd2;
          rrd6     = rrd2*rrd4;
          rrd7     = rrd6*rrd;
          epot    += (rrd6-rrd3);
          r148     = rrd7-0.5*rrd4;
          vir     -= rd*r148;
          forcex   = xx*r148;
          fxi     += forcex;
          #pragma omp atomic
          f[j]    -= forcex;
          forcey   = yy*r148;
          fyi     += forcey;
          #pragma omp atomic
          f[j+1]  -= forcey;
          forcez   = zz*r148;
          fzi     += forcez;
          #pragma omp atomic
          f[j+2]  -= forcez;
        }
      }
      #pragma omp atomic
      f[i]     += fxi;
      #pragma omp atomic
      f[i+1]   += fyi;
      #pragma omp atomic
      f[i+2]   += fzi;
    }
  }
