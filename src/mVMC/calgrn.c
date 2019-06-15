/*
mVMC - A numerical solver package for a wide range of quantum lattice models based on many-variable Variational Monte Carlo method
Copyright (C) 2016 The University of Tokyo, All rights reserved.

This program is developed based on the mVMC-mini program
(https://github.com/fiber-miniapp/mVMC-mini)
which follows "The BSD 3-Clause License".

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details. 

You should have received a copy of the GNU General Public License 
along with this program. If not, see http://www.gnu.org/licenses/. 
*/
/*-------------------------------------------------------------
 * Variational Monte Carlo
 * Cauculate Green Functions
 *-------------------------------------------------------------
 * by Satoshi Morita
 *-------------------------------------------------------------*/
#include "calgrn.h"
#ifndef _CALGRN_SRC
#define _CALGRN_SRC

int read_StdFace_L_W(){
  FILE * tmpfile;
  tmpfile = fopen("StdFace.def",  "r");
  //rewind(file);
    
  char tmpbuff[200];
  while(!feof(tmpfile)) 
  {
    if (fgets(tmpbuff,200,tmpfile))
    {
      if(sscanf (tmpbuff,"L = %d",&StdFace_L)) { }//printf("found L=%d\n", StdFace_L);} 
      if(sscanf (tmpbuff,"W = %d",&StdFace_W)) { } //printf("found W=%d\n", StdFace_W);} 
    }
  }
  assert(StdFace_L*StdFace_W==Nsite);
  
  fclose(tmpfile);
  //int r_out = moduloPython(ri+StdFace_W*delta_y + delta_x, Nsite);
  
  return 0;
}

// retunr i%N, python style
int moduloPython(int i,int N){
  return ((i % N) + N) % N;
}

int find_index_neighbor(int ri,int dx,int dy){
  int r_out = moduloPython(ri+StdFace_W*dx + dy, StdFace_W*StdFace_L);  
  return r_out;
}







double complex MultiplyFactor(double complex original,
                              int n, int *rsk, int *rsl,
                              int LeftFactor, int ri, int p,
                              int RightFactor, int rj, int q, 
                              int *eleNum, int PH) {
    
  int Factor = 1;
  PH = 0;
  // Multiply Factor from Right
  if(RightFactor==1){
    //if(!PH) 
    Factor = Factor * eleNum[rj+q*Nsite];
    //else    Factor = Factor * (1.0-eleNum[rj+q*Nsite]);
  }
  
  if(RightFactor==2){
    //if(!PH) 
    Factor = Factor * eleNum[rj]*eleNum[rj+Nsite];
    //else    Factor = Factor * (1.0-eleNum[rj])*(1.0-eleNum[rj+Nsite]);
  }
  
  int i;
  int rpi = ri+p*Nsite;
  //int unit = 1;// ((PH==0)? -1:1);

  // Multiply Factor from Left
  if(LeftFactor == 1){
    int modification = 0;
    for(i=0;i<n;i++){
      if(rsk[i]==rpi){modification += 1;}//(((PH==0)&&(i==0))? 1:-1);}
      if(rsl[i]==rpi){modification -= 1;}//(((PH==0)&&(i==0))? 1:-1);}
    }
    //if(!PH) 
    Factor = Factor * (eleNum[rpi]+modification);
    //else    Factor = Factor * (1.0-eleNum[rpi]-modification);
  }
  
  if(LeftFactor == 2){
    int spin;
    for(spin=0;spin<2;spin++){
      int modification = 0;
      for(i=0;i<n;i++){
        if(rsk[i]==(ri+spin*Nsite)){modification += 1;}//(((PH==0)&&(i==0))? 1:-1);}
        if(rsl[i]==(ri+spin*Nsite)){modification -= 1;}//(((PH==0)&&(i==0))? 1:-1);}
      }
      //if(!PH) 
      Factor = Factor * (eleNum[ri+spin*Nsite]+modification);
      //else    Factor = Factor * (1.0-eleNum[ri+spin*Nsite]-modification);
    }
  }

  return ((double)Factor *original);//TBC
  
}






double FactorRight(int ta, int rj, int q, int *eleNum) {
    
  int Factor = 1;
  
  //if(ta==-1){
    //Factor = Factor * eleNum[rj+q*Nsite];
  //}
  if(ta==0){
    Factor = Factor * eleNum[rj+q*Nsite];
  }
  else if(ta==1){
    Factor = Factor * eleNum[rj+(1-q)*Nsite];
  }
  else if(ta==2){
    Factor = Factor * eleNum[rj]*eleNum[rj+Nsite];
  }

  return ((double)Factor);  
}





double FactorLeft(int n, int *rsk, int *rsl,
                  int ta, int ri, int p,
                  int *eleNum) {
  if(ta == -1){
    return 1;
  }
  
  int ii;
  int Factor = 1;
  if(ta == 0){
    
    int rpi = ri+p*Nsite;
    int modification = 0;
    for(ii=0;ii<n;ii++){
      if(rsk[ii]==rpi){modification += 1;}//(((PH==0)&&(i==0))? 1:-1);}
      if(rsl[ii]==rpi){modification -= 1;}//(((PH==0)&&(i==0))? 1:-1);}
    }
    Factor = Factor * (eleNum[rpi]+modification);
  }
  else if(ta == 1){
    
    int rpi = ri+(1-p)*Nsite;
    int modification = 0;
    for(ii=0;ii<n;ii++){
      if(rsk[ii]==rpi){modification += 1;}//(((PH==0)&&(i==0))? 1:-1);}
      if(rsl[ii]==rpi){modification -= 1;}//(((PH==0)&&(i==0))? 1:-1);}
    }
    Factor = Factor * (eleNum[rpi]+modification);
  }
  else if(ta == 2){
    int spin;
    for(spin=0;spin<2;spin++){
      int modification = 0;
      for(ii=0;ii<n;ii++){
        if(rsk[ii]==(ri+spin*Nsite)){modification += 1;}//(((PH==0)&&(i==0))? 1:-1);}
        if(rsl[ii]==(ri+spin*Nsite)){modification -= 1;}//(((PH==0)&&(i==0))? 1:-1);}
      }
      //if(!PH) 
      Factor = Factor * (eleNum[ri+spin*Nsite]+modification);
    }
  }

  return ((double)Factor);
}



/* Function to calculate GFs with charge and doublon factors */
void MultiplyFactorGFs(int idx, int n, int *rsk, int *rsl,int *eleNum,
                        int flip, int PH,
      //
      double complex  AC_input, //input
      double complex  *nACm   // outputs
      ) {
                        
  // A: c     (annihilation operator)
  // C: c^dag (creation operator)
  
  int rm,rn,u;
  int ri,rj,s;
  
  ri = CisAjsIdx[idx][0];
  rj = CisAjsIdx[idx][2];
  s  = CisAjsIdx[idx][3];
  
  if (PH==1){
    ri = CisAjsIdx[idx][2];
    rj = CisAjsIdx[idx][0];
  }
  
  int idx_ex = ijst_to_idx[rj+s*Nsite][ri+s*Nsite]; 
  idx = ijst_to_idx[ri+s*Nsite][rj+s*Nsite]; 
  
  int N2 = NExcitation*NExcitation;
  int idx_exc_right,idx_exc_left;
  for(idx_exc_left=0;idx_exc_left<NExcitation;idx_exc_left++){
    int dr = ChargeExcitationIdx[idx_exc_left][1];
    int ta = ChargeExcitationIdx[idx_exc_left][0];
    int ra = (dr+rj+Nsite)%Nsite;
    int sa = s;
    double factor_left = FactorLeft(n,rsk,rsl,ta,ra,sa,eleNum);
    
    for(idx_exc_right=0;idx_exc_right<NExcitation;idx_exc_right++){
      int dr = ChargeExcitationIdx[idx_exc_right][1];
      int tb = ChargeExcitationIdx[idx_exc_right][0];
      int rb = (dr+ri+Nsite)%Nsite;
      int sb = s;
      double tmp = AC_input * factor_left * FactorRight(tb,rb,sb,eleNum);
      nACm[idx*N2    + idx_exc_right +  idx_exc_left*NExcitation] += tmp;
      //nACm[idx_ex*N2 + idx_exc_left  + idx_exc_right*NExcitation] += tmp;
    }
  }
  
} 




/* Function to calculate GFs with charge and doublon factors */
void MultiplyFactor2GFs(int idx, int NumElem, int n, int *rsk, int *rsl,int *eleNum,
                        int flip, int PH,
      //
      double complex  AC_input, //input
      //
      double complex  *AC,   // outputs
      double complex  *ACN,  // ...
      double complex  *ACM,  // 
      double complex  *ACD,
      //
      double complex *NAC,
      double complex *NACN,
      double complex *NACM,
      double complex *NACD,
      //
      double complex *MAC,
      double complex *MACN,
      double complex *MACM,
      double complex *MACD,
      //
      double complex *DAC,
      double complex *DACN,
      double complex *DACM,
      double complex *DACD) {

// to compute:
//  PhysAC, PhysACN, PhysACM, PhysACD,
// PhysNAC,PhysNACN,PhysNACM,PhysNACD,
// PhysMAC,PhysMACN,PhysMACM,PhysMACD,
// PhysDAC,PhysDACN,PhysDACM,PhysDACD
// from AC_input            
                        
  // A: c     (annihilation operator)
  // C: c^dag (creation operator)
  // N: n_s   (number operator same spin)
  // M: n_1-s (number operator opposite spin)
  // D: n_s * n_1-s (doublon operator)
  
  int rk,rl,s;
  int ri,p;
  int rj,q;
  
  rk = CisAjsIdx[idx][0];
  rl = CisAjsIdx[idx][2];
  s  = CisAjsIdx[idx][3];
  
  
  if (PH==1){
    rk = CisAjsIdx[idx][2];
    rl = CisAjsIdx[idx][0];
  }
  
  int idx_ex = ijst_to_idx[rk+s*Nsite][rl+s*Nsite]; 
  idx = ijst_to_idx[rl+s*Nsite][rk+s*Nsite]; 
  
  
  //PH = 1-PH;
  //PH=0;
  double complex tmp; //, tmp1, tmp2, tmp3;

  int idx_i,idx_j,idx_k,idx_l;
  AC[idx] += AC_input; 

  int sign[2] = {1,-1};
  int xx=0, yy=0;
  
    // ACN, NAC, ACM, MAC, ACD, DAC
    for(idx_k=0;idx_k<NNeighbors;idx_k++){
      
      rj = find_index_neighbor(rk, sign[xx]*neighbors_delta_x[idx_k], sign[yy]*neighbors_delta_y[idx_k]);
      double f_signs = 1.0;//0.25; 

      tmp = f_signs*MultiplyFactor(AC_input,n,rsk,rsl,0,0,0,1,rj,s,eleNum,  PH);
      ACN[idx+idx_k*NumElem] += tmp;
      NAC[idx_ex+idx_k*NumElem] += conj(tmp);

      tmp = f_signs*MultiplyFactor(AC_input,n,rsk,rsl,0,0,0,1,rj,1-s,eleNum, PH);
      ACM[idx+idx_k*NumElem] += tmp;
      MAC[idx_ex+idx_k*NumElem] += conj(tmp);

      tmp = f_signs*MultiplyFactor(AC_input,n,rsk,rsl,0,0,0,2,rj,0,eleNum, PH);
      ACD[idx+idx_k*NumElem] += tmp;
      DAC[idx_ex+idx_k*NumElem] += conj(tmp);
    }
    
    // NACN, NACD, DACN ...
    for(idx_l=0;idx_l<NNeighbors;idx_l++){
          
      ri = find_index_neighbor(rl, neighbors_delta_x[idx_l], neighbors_delta_y[idx_l]);
      
        for(idx_k=0;idx_k<NNeighbors;idx_k++){
                
          rj = find_index_neighbor(rk, neighbors_delta_x[idx_k], neighbors_delta_y[idx_k]);
          
          double    f_signs = 1.0;//0.25*0.25; 
          NACN[idx+(idx_l*NNeighbors+idx_k)*NumElem] += f_signs*MultiplyFactor(AC_input,n,rsk,rsl,1,ri,s,  1,rj,s,eleNum, PH);
          MACM[idx+(idx_l*NNeighbors+idx_k)*NumElem] += f_signs*MultiplyFactor(AC_input,n,rsk,rsl,1,ri,1-s,1,rj,1-s,eleNum, PH);
          DACD[idx+(idx_l*NNeighbors+idx_k)*NumElem] += f_signs*MultiplyFactor(AC_input,n,rsk,rsl,2,ri,0,  2,rj,0,eleNum, PH);
          
          tmp = f_signs*MultiplyFactor(AC_input,n,rsk,rsl,1,ri,s,1,rj,1-s,eleNum, PH);
          NACM[idx+(idx_l*NNeighbors+idx_k)*NumElem] += tmp;
          MACN[idx_ex+(idx_k*NNeighbors+idx_l)*NumElem] += conj(tmp);
          
          tmp = f_signs*MultiplyFactor(AC_input,n,rsk,rsl,1,ri,s,2,rj,0,eleNum, PH);
          NACD[idx+(idx_l*NNeighbors+idx_k)*NumElem] += tmp;
          DACN[idx_ex+(idx_k*NNeighbors+idx_l)*NumElem] += conj(tmp);
          
          tmp = f_signs*MultiplyFactor(AC_input,n,rsk,rsl,2,ri,0,1,rj,1-s,eleNum, PH);
          DACM[idx+(idx_l*NNeighbors+idx_k)*NumElem] += tmp;
          MACD[idx_ex+(idx_k*NNeighbors+idx_l)*NumElem] += conj(tmp);
        }
       //}
      //}       
    }
   //}
  //}
  
} // end of MultiplyFactor2GFs




int del(int i,int j){
  return ((i==j)? 1:0);
}




void CalculateGreenFuncMoments(const double w, const double complex ip, 
                               int *eleIdx, int *eleCfg,
                               int *eleNum, int *eleProjCnt) {

  int idx,idx0,idx1;
  int ri,rj,s,rk,rl,t;
  double complex tmp;
  int *myEleIdx, *myEleNum, *myProjCntNew;
  double complex *myBuffer;
  
  RequestWorkSpaceThreadInt(Nsize+Nsite2+NProj);
  //RequestWorkSpaceThreadComplex(NQPFull+2*Nsize);
  RequestWorkSpaceThreadComplex(NQPFull + 2*Nsize + 4*NCisAjs + 24*NCisAjs*NNeighbors + 36*NCisAjs*NNeighbors*NNeighbors);
  // GreenFunc1: NQPFull, GreenFunc2: NQPFull+2*Nsize 
  // Phys_AC_quantities: 4*NCisAjs + 24*NCisAjs*NNeighbors + 36*NCisAjs*NNeighbors*NNeighbors

  //#pragma omp parallel default(shared)                \
  private(myEleIdx,myEleNum,myProjCntNew,myBuffer,idx)
  {
    myEleIdx = GetWorkSpaceThreadInt(Nsize);
    myEleNum = GetWorkSpaceThreadInt(Nsite2);
    myProjCntNew = GetWorkSpaceThreadInt(NProj);
    myBuffer = GetWorkSpaceThreadComplex(NQPFull+2*Nsize);

    //#pragma loop noalias
    for(idx=0;idx<Nsize;idx++) myEleIdx[idx] = eleIdx[idx];
    //#pragma loop noalias
    for(idx=0;idx<Nsite2;idx++) myEleNum[idx] = eleNum[idx];

    //#pragma omp master
    {
      //printf("start:\n");
      StartTimer(50);
    }

    //#pragma omp for private(idx,ri,rj,s,tmp) schedule(dynamic) nowait
    for(idx=0;idx<NCisAjs;idx++) {
      ri = CisAjsIdx[idx][0];
      rj = CisAjsIdx[idx][2];
      s  = CisAjsIdx[idx][3];


      
      //Doublon-Holon TJS
      PhysN2[idx+NCisAjs*0] += w*myEleNum[ri+s*Nsite]*myEleNum[ri+(1-s)*Nsite]
                                   *(1.0-myEleNum[rj+s*Nsite])*(1.0-myEleNum[rj+(1-s)*Nsite]);
                                   
      //Doublon-Doublon TJS
      PhysN2[idx+NCisAjs*1] += w*myEleNum[ri+s*Nsite]*myEleNum[ri+(1-s)*Nsite]
                                      *myEleNum[rj+s*Nsite]*myEleNum[rj+(1-s)*Nsite];

      //Charge-Doublon TJS
      PhysN2[idx+NCisAjs*2] += w*myEleNum[ri+s*Nsite] *myEleNum[rj+s*Nsite]*myEleNum[rj+(1-s)*Nsite];
      
      //n_sigma (1-n_sigma) MC
      PhysN2[idx+NCisAjs*3] += w*myEleNum[ri+s*Nsite] *(1.0-myEleNum[rj+s*Nsite]);
    }

    //#pragma omp for private(ri,s) schedule(dynamic) nowait
    for(ri=0;ri<Nsite;ri++) {
      s=0;
      //Doublon MC
      PhysN1[ri+Nsite*0] += w*myEleNum[ri]*myEleNum[ri+Nsite];

      //Holon MC
      PhysN1[ri+Nsite*1] += w*(1.0-myEleNum[ri])*(1.0-myEleNum[ri+Nsite]);
            
      //Density_up (s==0) MC
      PhysN1[ri+Nsite*2] += w*myEleNum[ri];

      //Density_down (s==1) MC
      PhysN1[ri+Nsite*3] += w*myEleNum[ri+Nsite];           
    }
    
    //#pragma omp master
    {StopTimer(50);}

  


   
  double factor;
  int rsk1[1], rsl1[1];
  int rsk2[2], rsl2[2];

  /*Local two-body Green's fuction LocalCktAltCmuAnu TJS*/
  int idx_int, idx_trans;
  int rm, rn, u;


//#pragma omp for private(idx,ri,rj,s,tmp) schedule(dynamic) nowait
  for(idx=0;idx<NCisAjs;idx++) {
    ri = CisAjsIdx[idx][0];
    rj = CisAjsIdx[idx][2];
    s  = CisAjsIdx[idx][3];
    tmp = GreenFunc1(ri,rj,s,ip,myEleIdx,eleCfg,myEleNum,eleProjCnt,
                     myProjCntNew,myBuffer);
    LocalCisAjs[idx] = tmp;
  }

  int ii;
  for (ii = 0; ii < Nsite; ii++) {
    printf("%d", myEleNum[ii]);
  }
  printf(" ");
  for (ii = 0; ii < Nsite; ii++) {
    printf("%d", myEleNum[ii+Nsite]);
  }
  printf("  %f %f ",creal(LocalCisAjs[0]), cimag(LocalCisAjs[0]));
  printf("  %f %f ",creal(LocalCisAjs[1]), cimag(LocalCisAjs[1]));
  printf("\n");

//#pragma omp for private(idx) nowait
  for(idx=0;idx<NCisAjs;idx++) {
    PhysCisAjs[idx] += w*LocalCisAjs[idx];
  }

//#pragma omp for private(idx,idx_trans,rk,rl,t,rm,rn,u) schedule(dynamic) nowait
  for(idx=0;idx<NCisAjs;idx++) {
    rk = CisAjsIdx[idx][0];
    rl = CisAjsIdx[idx][2];
    t  = CisAjsIdx[idx][3];

    for(idx_trans=0;idx_trans<NTransfer;idx_trans++) {
    
      rm = Transfer[idx_trans][0];
      rn = Transfer[idx_trans][2];
      u  = Transfer[idx_trans][3];
        
      LocalCktAltCmuAnu[idx_trans][idx] = GreenFunc2(rk,rl,rm,rn,t,u,ip,
             myEleIdx,eleCfg,myEleNum,eleProjCnt,myProjCntNew,myBuffer);
    }
  }
//*
    // <c|a>, <a|c>
//#pragma omp for private(idx,ri,rj,s,rk,rl,t,rsk1,rsl1,tmp) schedule(dynamic) nowait
    for(idx=0;idx<NCisAjs;idx++) {

      rk = CisAjsIdx[idx][0];
      rl = CisAjsIdx[idx][2];
      s  = CisAjsIdx[idx][3];

      rsk1[0] = rk+s*Nsite;
      rsl1[0] = rl+s*Nsite;


      // <c|a>
      // Composite Hole correlation function TJS + MC
      tmp = 1.*LocalCisAjs[idx];
//      MultiplyFactorGFs(idx,1,&rsk1[0],&rsl1[0],myEleNum,0,1,w*tmp,Phys_nCAm);
      MultiplyFactor2GFs(idx,NCisAjs,1,&rsk1[0],&rsl1[0],myEleNum,0,1,w*tmp,
                          PhysCA, PhysCAN, PhysCAM, PhysCAD,
                         PhysNCA,PhysNCAN,PhysNCAM,PhysNCAD,
                         PhysMCA,PhysMCAN,PhysMCAM,PhysMCAD,
                         PhysDCA,PhysDCAN,PhysDCAM,PhysDCAD);

      // <a|c>
      // Composite Fermion correlation functions TJS + MC
      tmp =  1.*del(rk,rl) - 1.*LocalCisAjs[idx];
//      MultiplyFactorGFs(idx,1,&rsk1[0],&rsl1[0],myEleNum,0,0,w*tmp,Phys_nACm);
      MultiplyFactor2GFs(idx,NCisAjs,1,&rsk1[0],&rsl1[0],myEleNum,0,0,w*tmp,
                          PhysAC, PhysACN, PhysACM, PhysACD,
                         PhysNAC,PhysNACN,PhysNACM,PhysNACD,
                         PhysMAC,PhysMACN,PhysMACM,PhysMACD,
                         PhysDAC,PhysDACN,PhysDACM,PhysDACD);
    }
    
    // <c|H|a>, <a|H|c> 
    // where H = H_U + H_T    
    double complex tmp_int_AHC = 0.0,   tmp_int_CHA = 0.0;
    double complex tmp_trans_AHC = 0.0, tmp_trans_CHA = 0.0;
    
    //*
//#pragma omp for private(idx,idx_int,idx_trans,rsk1,rsl1,rsk2,rsl2,s,rk,rl,t,rm,rn,u,tmp,tmp_int_AHC,tmp_int_CHA,tmp_trans_AHC,tmp_trans_CHA,factor) schedule(dynamic) nowait
    for(idx=0;idx<NCisAjs;idx++) {
      
      rk = CisAjsIdx[idx][0];
      rl = CisAjsIdx[idx][2];
      s  = CisAjsIdx[idx][3];
      
      //assumig the same spin for Ck and Al
      t = s;

      rsk1[0] = rk+s*Nsite;
      rsl1[0] = rl+s*Nsite;

      tmp_int_AHC=0.0;
      tmp_int_CHA=0.0;
      for(idx_int=0;idx_int<NCoulombIntra;idx_int++) {        

        rm = CoulombIntra[idx_int];
        factor = ParaCoulombIntra[idx_int]*
                 ( 1.*del(rk,rm) + myEleNum[rm+s*Nsite])*myEleNum[rm+(1-s)*Nsite];

        tmp_int_AHC += factor*( 1.*del(rk,rl) - LocalCisAjs[idx]);        

        factor = ParaCoulombIntra[idx_int]*
                 (-1.*del(rl,rm) + myEleNum[rm+s*Nsite])*myEleNum[rm+(1-s)*Nsite];
    
        tmp_int_CHA += factor*LocalCisAjs[idx];
      }

      // <a|H_U|c>    
//      MultiplyFactorGFs(idx,1,&rsk1[0],&rsl1[0],myEleNum,0,0,w*tmp_int_AHC,Phys_nAHCm);
      MultiplyFactor2GFs(idx,NCisAjs,1,&rsk1[0],&rsl1[0],myEleNum,0,0, w*tmp_int_AHC,
                          PhysAHC, PhysAHCN, PhysAHCM, PhysAHCD,
                         PhysNAHC,PhysNAHCN,PhysNAHCM,PhysNAHCD,
                         PhysMAHC,PhysMAHCN,PhysMAHCM,PhysMAHCD,
                         PhysDAHC,PhysDAHCN,PhysDAHCM,PhysDAHCD);

      // <c|H_U|a>    
//      MultiplyFactorGFs(idx,1,&rsk1[0],&rsl1[0],myEleNum,0,1,w*tmp_int_CHA,Phys_nCHAm);
      MultiplyFactor2GFs(idx,NCisAjs,1,&rsk1[0],&rsl1[0],myEleNum,0,1, w*tmp_int_CHA,
                          PhysCHA, PhysCHAN, PhysCHAM, PhysCHAD,
                         PhysNCHA,PhysNCHAN,PhysNCHAM,PhysNCHAD,
                         PhysMCHA,PhysMCHAN,PhysMCHAM,PhysMCHAD,
                         PhysDCHA,PhysDCHAN,PhysDCHAM,PhysDCHAD);
                               
      // <c|H_T|a> , <a|H_T|c>  
      rsk2[0] = rk+s*Nsite;
      rsl2[0] = rl+s*Nsite;

      tmp_trans_AHC = 0.0;
      tmp_trans_CHA = 0.0;
      for(idx_trans=0;idx_trans<NTransfer;idx_trans++) {
        
        rm = Transfer[idx_trans][0];
        rn = Transfer[idx_trans][2];
        u  = Transfer[idx_trans][3];

        rsk2[1] = rm+u*Nsite;
        rsl2[1] = rn+u*Nsite;

        // <a|H_T|c> 
        factor = ParaTransfer[idx_trans];
        tmp = factor*LocalCktAltCmuAnu[idx_trans][idx];

        if(rk==rl){
          tmp += -factor*LocalCisAjs[ijst_to_idx[rm+u*Nsite][rn+u*Nsite]];
        }
        if((rk==rn)&&(s==u)){
          tmp += factor*LocalCisAjs[ijst_to_idx[rm+u*Nsite][rl+s*Nsite]];
          if((rl==rm)&&(s==u)){
            tmp += -factor;
          }
        }
        
        tmp_trans_AHC += tmp;
//        MultiplyFactorGFs(idx,2,&rsk2[0],&rsl2[0],myEleNum,0,0,w*tmp,Phys_nAHCm);
        MultiplyFactor2GFs(idx,NCisAjs,2,&rsk2[0],&rsl2[0],myEleNum,0,0, w*tmp,
                            PhysAHC, PhysAHCN, PhysAHCM, PhysAHCD,
                           PhysNAHC,PhysNAHCN,PhysNAHCM,PhysNAHCD,
                           PhysMAHC,PhysMAHCN,PhysMAHCM,PhysMAHCD,
                           PhysDAHC,PhysDAHCN,PhysDAHCM,PhysDAHCD);           
        
        // <c|H_T|a>  
        factor = ParaTransfer[idx_trans];
        tmp = -1.0*factor*LocalCktAltCmuAnu[idx_trans][idx];        
        if((rl==rm)&&(s==u)){
          tmp += +factor*LocalCisAjs[ijst_to_idx[rk+s*Nsite][rn+u*Nsite]];
        }

        tmp_trans_CHA += tmp;
//        MultiplyFactorGFs(idx,2,&rsk2[0],&rsl2[0],myEleNum,0,1,w*tmp,Phys_nCHAm);
        MultiplyFactor2GFs(idx,NCisAjs,2,&rsk2[0],&rsl2[0],myEleNum,0,1, w*tmp,
                            PhysCHA, PhysCHAN, PhysCHAM, PhysCHAD,
                           PhysNCHA,PhysNCHAN,PhysNCHAM,PhysNCHAD,
                           PhysMCHA,PhysMCHAN,PhysMCHAM,PhysMCHAD,
                           PhysDCHA,PhysDCHAN,PhysDCHAM,PhysDCHAD);
      }
      
    }//*/
  }  
  ReleaseWorkSpaceThreadInt();
  ReleaseWorkSpaceThreadComplex();
  return;
}











int Commute_Nat_(commuting_with commuting, int ra, int rb, int sa, int t, int ri, int rj, int s, int rm, int rn, int u, int *eleNum) {
  int sign;
  if((commuting==with_CisCmuAnuAjs) || (commuting==with_AisCmuAnuCjs)) {
    if(commuting==with_CisCmuAnuAjs) {sign = 1;}
    else if(commuting==with_AisCmuAnuCjs) {sign = -1;}
    
    if(t==-1){ //no charge
      return 1;
    }
    else if(t==0){ // electron same-spin
      return eleNum[ra+sa*Nsite]     + sign * del(sa,s) * (del(ra,ri)-del(ra,rj))     
                                            + del(sa,u) * (del(ra,rm)-del(ra,rn));
    }
    else if(t==1){ // electron reverse-spin
      return eleNum[ra+(1-sa)*Nsite] + sign * del((1-sa),s) * (del(ra,ri)-del(ra,rj)) 
                                            + del((1-sa),u) * (del(ra,rm)-del(ra,rn));
    }
    else if(t==2){ // doublon
      return (eleNum[ra]       + sign * (del(ra,ri)-del(ra,rj) ) * del(s,0) + (del(ra,rm)-del(ra,rn) ) * del(u,0) )
            *(eleNum[ra+Nsite] + sign * (del(ra,ri)-del(ra,rj) ) * del(s,1) + (del(ra,rm)-del(ra,rn) ) * del(u,1) );
    }
    else if(t==3){
      return (eleNum[ra+(1-sa)*Nsite] + sign * (del(ra,ri)-del(ra,rj) ) * del(s,1-sa) + (del(ra,rm)-del(ra,rn) ) * del(u,1-sa) )
            *(eleNum[rb+(1-sa)*Nsite] + sign * (del(rb,ri)-del(rb,rj) ) * del(s,1-sa) + (del(rb,rm)-del(rb,rn) ) * del(u,1-sa) );
    }
    else if(t==4){
      return (eleNum[ra+(1-sa)*Nsite] + sign * (del(ra,ri)-del(ra,rj) ) * del(s,1-sa) + (del(ra,rm)-del(ra,rn) ) * del(u,1-sa) )
            *(eleNum[rb+   sa *Nsite] + sign * (del(rb,ri)-del(rb,rj) ) * del(s,sa)   + (del(rb,rm)-del(rb,rn) ) * del(u,sa) );
    }
    else{
      printf("oups, error\n");
      exit(1);
      return -1;
    }
  
  }
  else if((commuting==with_CisAjs) || (commuting==with_AisCjs)) {
  
    if(commuting==with_CisAjs) {sign = 1;}
    else if(commuting==with_AisCjs) {sign = -1;}
    
    if(t==-1){ //no charge
      // <phi| c_is a_js |x>  =  <phi| c_is a_js |x>  
      // <phi| a_is c_js |x>  =  <phi| a_is c_js |x> 
      return 1;
    }
    else if(t==0){ // electron same-spin
      // <phi| n_a:sa c_is a_js |x>  =  <phi| c_is a_js |x> (n_a:sa(x) + del_sa,s*(del_a,i - del_a,j)) 
      // <phi| n_a:sa a_is c_js |x>  =  <phi| a_is c_js |x> (n_a:sa(x) - del_sa,s*(del_a,i - del_a,j)) 
      return eleNum[ra+sa*Nsite]     + sign * del(sa,s)     * (del(ra,ri)-del(ra,rj));
    }
    else if(t==1){ // electron reverse-spin
      // <phi| n_a:sa c_is a_js |x>  =  <phi| c_is a_js |x> (n_a:sa(x) + del_-sa,s*(del_a,i - del_a,j)) 
      // <phi| n_a:sa a_is c_js |x>  =  <phi| a_is c_js |x> (n_a:sa(x) - del_-sa,s*(del_a,i - del_a,j)) 
      return eleNum[ra+(1-sa)*Nsite] + sign * del((1-sa),s) * (del(ra,ri)-del(ra,rj));
    }
    else if(t==2){ // doublon
      // <phi| n_a:up n_a:dn c_is a_js |x>  =  <phi| c_is a_js |x> (n_a:up(x) n_a:dn(x) + n_a:-s (del_a,i - del_a,j)) 
      // <phi| n_a:up n_a:dn a_is c_js |x>  =  <phi| a_is c_js |x> (n_a:up(x) n_a:dn(x) - n_a:-s (del_a,i - del_a,j)) 
      return (eleNum[ra]       + sign * (del(ra,ri)-del(ra,rj) ) * del(s,0) )
            *(eleNum[ra+Nsite] + sign * (del(ra,ri)-del(ra,rj) ) * del(s,1) );
            //eleNum[ra]*eleNum[ra+Nsite] + sign * eleNum[ra+(1-s)*Nsite] * (del(ra,ri)-del(ra,rj));
    }
    else if(t==3){ 
      return (eleNum[ra+(1-sa)*Nsite] + sign * (del(ra,ri)-del(ra,rj) ) * del(s,1-sa) )
            *(eleNum[rb+(1-sa)*Nsite] + sign * (del(rb,ri)-del(rb,rj) ) * del(s,1-sa) );
    }
    else if(t==4){ 
      return (eleNum[ra+(1-sa)*Nsite] + sign * (del(ra,ri)-del(ra,rj) ) * del(s,1-sa) )
            *(eleNum[rb+    sa*Nsite] + sign * (del(rb,ri)-del(rb,rj) ) * del(s,sa) );
    }
    /*
    else if(t==3){ // hole same-spin
      // <phi| (1-n_a:sa) c_is a_js |x>  =  <phi| c_is a_js |x> (1- (n_a:sa(x) + del_sa,s*(del_a,i - del_a,j)))
      // <phi| (1-n_a:sa) a_is c_js |x>  =  <phi| a_is c_js |x> (1- (n_a:sa(x) - del_sa,s*(del_a,i - del_a,j))) 
      return 1 - (eleNum[ra+sa*Nsite] + sign * del(sa,s) * (del(ra,ri)-del(ra,rj)));
    }
    else if(t==4){ // hole reverse-spin
      // <phi| (1-n_a:sa) c_is a_js |x>  =  <phi| c_is a_js |x> (1- (n_a:sa(x) + del_-sa,s*(del_a,i - del_a,j)))
      // <phi| (1-n_a:sa) a_is c_js |x>  =  <phi| a_is c_js |x> (1- (n_a:sa(x) - del_-sa,s*(del_a,i - del_a,j)))
      return 1 - (eleNum[ra+(1-sa)*Nsite] + sign * del((1-sa),s) * (del(ra,ri)-del(ra,rj)));
    }
    else if(t==5){ // holon
      // <phi| (1-n_a:sa) (1-n_a:-sa) c_is a_js |x>  =  <phi| c_is a_js |x> (1-n_a:-sa(x)) ((1-n_a:dn(x)) - n_a:-s (del_a,i - del_a,j)))
      // <phi| (1-n_a:sa) (1-n_a:-sa) a_is c_js |x>  =  <phi| a_is c_js |x> (1-n_a:-sa(x)) ((1-n_a:dn(x)) + n_a:-s (del_a,i - del_a,j))) 
      return (1 - eleNum[ra+(1-s)*Nsite]) * (1 - eleNum[ra+s*Nsite] - sign * (del(ra,ri)-del(ra,rj) ));
    }*/
    else{
      printf("oups, error\n");
      exit(1);
      return -1;
    }
  }
  else if(commuting==with_nothing) {
    if(t==-1){ //no charge
      return 1;
    }
    else if(t==0){ //0 electron same-spin
      return eleNum[ra+sa*Nsite];
    }
    else if(t==1){ //1 electron reverse-spin
      return eleNum[ra+(1-sa)*Nsite];
    }
    else if(t==2){ // doublon
      return (eleNum[ra]*eleNum[ra+Nsite]);
    }
    else if(t==3){ 
      if(ra==rb) {
        printf("oups, ra==rb\n");
        exit(1);
      }
      return (eleNum[ra+(1-sa)*Nsite]*eleNum[rb+(1-sa)*Nsite]);
    }
    else if(t==4){ 
      if(rb==ra) {
        printf("oups, rb==ra\n");
        exit(1);
      }
      return (eleNum[ra+(1-sa)*Nsite]*eleNum[rb+sa*Nsite]);
    }
    /*
    else if(t==3){ // hole same-spin
      return (1-eleNum[ra+sa*Nsite]);
    }
    else if(t==4){ // hole opposite-spin
      return (1-eleNum[ra+(1-sa)*Nsite]);
    }
    else if(t==5){ // holon
      return ((1-eleNum[ra])*(1-eleNum[ra+Nsite]));
    }*/
    else{
      printf("oups, error\n");
      exit(1);
      return -1;
    }
  }
  
}






//C=C+weight*A*B
unsigned int C_ADD_AxB(double complex * C, double complex const * A, double complex const * B, int N, double complex weight) {
  char transA= 'N', transB= 'C';
  double complex beta=1.0;
  int ONE = 1;
  M_ZGEMM(&transA,&transB,&N,&N,&ONE, &weight, &A[0], &N, &B[0], &N, &beta, &C[0], &N); 
  return 0;
}


void CalculateGreenFuncMoments2(const double w, const double complex ip, 
                                int *eleIdx, int *eleCfg,
                                int *eleNum, int *eleProjCnt) {

  int idx,idx0,idx1;
  int ri,rj,s,rk,rl,t;
  double complex tmp;
  int *myEleIdx, *myEleNum, *myProjCntNew;
  double complex *myBuffer;
  
  RequestWorkSpaceThreadInt(Nsize+Nsite2+NProj);
  RequestWorkSpaceThreadComplex(NQPFull + 2*Nsize + 10*NCisAjs*NExcitation + 2*NCisAjs);
  
  double complex *O_AC_vec1, *O_CA_vec1, AC_tmp, CA_tmp;//, *H_vec;//, *O_vec;
  double complex *O_AC_vec2, *O_CA_vec2;
  double complex *O0_vec1, *O0_vec2;
  double complex *H_AC_vec1, *H_CA_vec1;
  double complex *H_AC_vec2, *H_CA_vec2;
  
  //#pragma omp parallel default(shared)                \
  //private(myEleIdx,myEleNum,myProjCntNew,myBuffer,idx, O_AC_vec, O_CA_vec, O0_vec, H_vec)//, O_vec)
  {
    O_AC_vec1 = GetWorkSpaceThreadComplex(NCisAjs*NExcitation);
    O_CA_vec1 = GetWorkSpaceThreadComplex(NCisAjs*NExcitation);
    O_AC_vec2 = GetWorkSpaceThreadComplex(NCisAjs*NExcitation);
    O_CA_vec2 = GetWorkSpaceThreadComplex(NCisAjs*NExcitation);
    //O0_AC_vec = GetWorkSpaceThreadComplex(NCisAjs*NExcitation);
    O0_vec1 = GetWorkSpaceThreadComplex(NCisAjs*NExcitation);
    O0_vec2 = GetWorkSpaceThreadComplex(NCisAjs*NExcitation);
    //H0_AC_vec = GetWorkSpaceThreadComplex(NCisAjs*NExcitation);
    H_AC_vec1 = GetWorkSpaceThreadComplex(NCisAjs*NExcitation);
    H_CA_vec1 = GetWorkSpaceThreadComplex(NCisAjs*NExcitation);
    H_AC_vec2 = GetWorkSpaceThreadComplex(NCisAjs*NExcitation);
    H_CA_vec2 = GetWorkSpaceThreadComplex(NCisAjs*NExcitation);

    
    myEleIdx = GetWorkSpaceThreadInt(Nsize);
    myEleNum = GetWorkSpaceThreadInt(Nsite2);
    myProjCntNew = GetWorkSpaceThreadInt(NProj);
    myBuffer = GetWorkSpaceThreadComplex(NQPFull+2*Nsize);

    //#pragma loop noalias
    for(idx=0;idx<Nsize;idx++) myEleIdx[idx] = eleIdx[idx];
    //#pragma loop noalias
    for(idx=0;idx<Nsite2;idx++) myEleNum[idx] = eleNum[idx];

    
    double factor;
    int rm, rn, u;
    int idx_int, idx_trans;


//#pragma omp for private(idx,ri,rj,s,tmp) schedule(dynamic) nowait
    for(idx=0;idx<NCisAjs;idx++) {
      ri = CisAjsIdx[idx][0];
      rj = CisAjsIdx[idx][2];
      s  = CisAjsIdx[idx][3];
      tmp = GreenFunc1(ri,rj,s,ip,myEleIdx,eleCfg,myEleNum,eleProjCnt,
                       myProjCntNew,myBuffer);
      LocalCisAjs[idx] = tmp;
    }

//#pragma omp for private(idx) nowait
    for(idx=0;idx<NCisAjs;idx++) {
      PhysCisAjs[idx] += w*LocalCisAjs[idx];
    }


//#pragma omp for private(idx,idx_trans,rk,rl,t,rm,rn,u) schedule(dynamic) nowait
    for(idx=0;idx<NCisAjs;idx++) {
      rk = CisAjsIdx[idx][0];
      rl = CisAjsIdx[idx][2];
      t  = CisAjsIdx[idx][3];

      for(idx_trans=0;idx_trans<NTransfer;idx_trans++) {
      
        rm = Transfer[idx_trans][0];
        rn = Transfer[idx_trans][2];
        u  = Transfer[idx_trans][3];
          
        LocalCktAltCmuAnu[idx_trans][idx] = GreenFunc2(rk,rl,rm,rn,t,u,ip,
               myEleIdx,eleCfg,myEleNum,eleProjCnt,myProjCntNew,myBuffer);
      }
    }
    
    /*
    printf("              ");
    for(idx_trans=0;idx_trans<NTransfer;idx_trans++) {
      
        rm = Transfer[idx_trans][0];
        rn = Transfer[idx_trans][2];
        u  = Transfer[idx_trans][3];
    
        printf("%d%d%d%d   ", rm, u, rn, u);
          
    }
    printf("\n");
    
    for (idx = 0; idx < 2*Nsite; idx++) {
      printf("%d", myEleNum[idx]);
    }
    printf("\n");
    
    for(idx=0;idx<NCisAjs;idx++) {
      printf("%d %d %d %d     ", CisAjsIdx[idx][0], CisAjsIdx[idx][1], CisAjsIdx[idx][2], CisAjsIdx[idx][3]);
      for(idx_trans=0;idx_trans<NTransfer;idx_trans++) {
        printf("% 5.3f ", creal(LocalCktAltCmuAnu[idx_trans][idx]) );
      }
      printf("\n");
    }
    printf("\n");
    */
///*
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//#pragma omp for private(idx,ri,rj,s,rk,rl,t,tmp) schedule(dynamic) nowait    
    for(idx=0;idx<NCisAjs;idx++) {
      ri = CisAjsIdx[idx][0];
      rj = CisAjsIdx[idx][2];
      s  = CisAjsIdx[idx][3];
      double complex tmp_CA =  1.*LocalCisAjs[idx];
      int idx_exc;
      for(idx_exc=0;idx_exc<NExcitation;idx_exc++){
        //int idx_vector = idx+idx_exc*NCisAjs;
        int idx_vector = idx_exc + idx*NExcitation;
        int idx_green = ijst_to_idx[rj+s*Nsite][ri+s*Nsite];
        int dr1 = ChargeExcitationIdx[idx_exc][1];
        int dr2 = ChargeExcitationIdx[idx_exc][2];
        int t   = ChargeExcitationIdx[idx_exc][0];
        int ra1 = (dr1+ri+Nsite)%Nsite;
        int ra2 = (dr2+ri+Nsite)%Nsite;
        int rb1 = (dr1+rj+Nsite)%Nsite;
        int rb2 = (dr2+rj+Nsite)%Nsite;
        int idx_vectorEx = idx_exc + idx_green*NExcitation;
        
        // <phi|ac|x> / <phi|x> = delta_{ri,rj} * <phi|x> / <phi|x> - <phi|ca|x> / <phi|x>           <-- need to reverse indices
        AC_tmp  = del(ri,rj);
        AC_tmp -= LocalCisAjs[idx_green];
        AC_tmp *= ((double) (Commute_Nat_(with_AisCjs, ra1, ra2, s, t, ri, rj, s, 0,0,0, myEleNum)));
        O_AC_vec1[idx_vector]   = AC_tmp;
        O_AC_vec2[idx_vectorEx] = conj(AC_tmp);
        
        // <phi|ca|x> / <phi|x>
        CA_tmp = LocalCisAjs[idx] * ((double) (Commute_Nat_(with_CisAjs,  ra1, ra2, s, t, ri, rj, s, 0,0,0, myEleNum)));        
        O_CA_vec1[idx_vector]   = CA_tmp;        
        O_CA_vec2[idx_vectorEx] = conj(CA_tmp);        
        
        // <phi|x>
        O0_vec1[idx_vector]   = ((double) (Commute_Nat_(with_nothing, rb1, rb2, s, t,  0,  0, 0, 0,0,0, myEleNum)));
        O0_vec2[idx_vectorEx] = ((double) (Commute_Nat_(with_nothing, rb1, rb2, s, t,  0,  0, 0, 0,0,0, myEleNum)));
        //*
        // <phi|H_U|x> 
        double tmp_int_AHC=0.0;
        double tmp_int_CHA=0.0;
        
        
        for(idx_int=0;idx_int<NCoulombIntra;idx_int++) {
          rm = CoulombIntra[idx_int];
          factor = ParaCoulombIntra[idx_int] *
                   ( 1.*del(rj,rm) + myEleNum[rm+s*Nsite])*myEleNum[rm+(1-s)*Nsite];
          tmp_int_AHC += factor;        

          factor = ParaCoulombIntra[idx_int] *
                   ( -1.*del(rj,rm) + myEleNum[rm+s*Nsite])*myEleNum[rm+(1-s)*Nsite];
          tmp_int_CHA += factor;
        }
        /*
        H_CA_vec1[idx_vector] = 0.0;
        H_AC_vec1[idx_vector] = 0.0;
        H_CA_vec2[idx_vectorEx] = 0.0;
        H_AC_vec2[idx_vectorEx] = 0.0;
        */
        
        H_AC_vec1[idx_vector] =  tmp_int_AHC * AC_tmp ;
        H_CA_vec1[idx_vector] =  tmp_int_CHA * CA_tmp ;
        
        H_AC_vec2[idx_vectorEx] = conj(tmp_int_AHC * AC_tmp) ;
        H_CA_vec2[idx_vectorEx] = conj(tmp_int_CHA * CA_tmp) ;
        //*
        // <phi|H_T|x> / <phi|x>
        for(idx_trans=0;idx_trans<NTransfer;idx_trans++) {
          
          rm = Transfer[idx_trans][0];
          rn = Transfer[idx_trans][2];
          u  = Transfer[idx_trans][3];
          
          int idx_green0 = ijst_to_idx[ri+s*Nsite][rn+s*Nsite];
          tmp = -1.0 * ParaTransfer[idx_trans] 
                     * (LocalCktAltCmuAnu[idx_trans][idx] 
                        - del(rm,rj) * del(s,u) * LocalCisAjs[idx_green0]) 
                     * ((double) (Commute_Nat_(with_CisCmuAnuAjs, ra1, ra2, s, t, ri, rj, s, rm, rn, u, myEleNum))) ;
          H_CA_vec1[idx_vector]   += tmp;
          H_CA_vec2[idx_vectorEx] += conj(tmp);
          
          int idx_green1 = ijst_to_idx[rj+s*Nsite][ri+s*Nsite];
          int idx_green2 = ijst_to_idx[rm+u*Nsite][rn+u*Nsite];
          int idx_green3 = ijst_to_idx[rm+u*Nsite][ri+u*Nsite];
          tmp = -1.0 * ParaTransfer[idx_trans] 
                     * ( - LocalCktAltCmuAnu[idx_trans][idx_green1] 
                         + del(ri,rj) * LocalCisAjs[idx_green2] 
                         + del(rn,rj) * del(s,u) * ( del(rm,ri) - LocalCisAjs[idx_green3] ))
                     * ((double) (Commute_Nat_(with_AisCmuAnuCjs, ra1, ra2, s, t, ri, rj, s, rm, rn, u, myEleNum))) ;
          H_AC_vec1[idx_vector]   += tmp;
          H_AC_vec2[idx_vectorEx] += conj(tmp);
          
        }
        ///
      }
    }
    
    
    int ii, jj,nn,mm;
    //*
    int N  = NExcitation;
    int N2 = NExcitation*NExcitation;
    double complex alpha = 0.5*w;
    for (ii = 0; ii < NCisAjs; ii++) {

      C_ADD_AxB(&Phys_nACm[ii*N2], &O_AC_vec1[ii*N], &O0_vec1[ii*N], N, alpha);
      C_ADD_AxB(&Phys_nCAm[ii*N2], &O_CA_vec1[ii*N], &O0_vec1[ii*N], N, alpha);
      
      C_ADD_AxB(&Phys_nACm[ii*N2], &O0_vec2[ii*N], &O_AC_vec2[ii*N], N, alpha);
      C_ADD_AxB(&Phys_nCAm[ii*N2], &O0_vec2[ii*N], &O_CA_vec2[ii*N], N, alpha);
      
      C_ADD_AxB(&Phys_nAHCm[ii*N2], &H_AC_vec1[ii*N], &O0_vec1[ii*N], N, alpha);
      C_ADD_AxB(&Phys_nCHAm[ii*N2], &H_CA_vec1[ii*N], &O0_vec1[ii*N], N, alpha);
      
      C_ADD_AxB(&Phys_nAHCm[ii*N2], &O0_vec2[ii*N], &H_AC_vec2[ii*N], N, alpha);
      C_ADD_AxB(&Phys_nCHAm[ii*N2], &O0_vec2[ii*N], &H_CA_vec2[ii*N], N, alpha);
    }
    //*/
    
    /*
    for (ii = 0; ii < Nsite; ii++) {
      printf("%d", myEleNum[ii]);
    }
    printf(" ");
    for (ii = 0; ii < Nsite; ii++) {
      printf("%d", myEleNum[ii+Nsite]);
    }
    printf("  %f %f ",creal(LocalCisAjs[0]), cimag(LocalCisAjs[0]));
    printf("  %f %f ",creal(LocalCisAjs[1]), cimag(LocalCisAjs[1]));
    printf("\n");
    /*
    printf("\n");
    for (ii = 0; ii < NCisAjs; ii++) {
     for (nn = 0; nn < N; nn++) {
      for (mm = 0; mm < N; mm++) {
        printf("% 6.2f ",creal(Phys_nAHCm[ii*N + jj]));
      }
      printf("\n");
     }
     printf("\n");
    }
    printf("\n");
    */

    
    //printf("  %f %f ",creal(LocalCisAjs[0]), cimag(LocalCisAjs[0]));
    //printf("  %f %f ",creal(LocalCisAjs[1]), cimag(LocalCisAjs[1]));
    //printf("\n");
    
    
  } //
  
  

  ReleaseWorkSpaceThreadInt();
  ReleaseWorkSpaceThreadComplex();
  return;
}





/*
void CalculateGreenFuncMoments3(const double w, const double complex ip, 
                                int *eleIdx, int *eleCfg,
                                int *eleNum, int *eleProjCnt) {

  int idx,idx0,idx1;
  int ri,rj,s,rk,rl,t;
  double complex tmp;
  int *myEleIdx, *myEleNum, *myProjCntNew;
  double complex *myBuffer;
  
  RequestWorkSpaceThreadInt(Nsize+Nsite2+NProj);
  RequestWorkSpaceThreadComplex(NQPFull + 2*Nsize + 4*NCisAjs*NExcitation + 2*NCisAjs + 2*NExcitation);
  
  double complex *O_AC_vec, *O_CA_vec;//, *H_vec;//, *O_vec;
  doubmle complex *O0_vec;
  double complex *H0_vec;
  double complex *W2_vec, *one_over_W2_vec;
  double complex *O_AC_corr_n, *O_CA_corr_n;
  
  
  //#pragma omp parallel default(shared)                \
  //private(myEleIdx,myEleNum,myProjCntNew,myBuffer,idx, O_AC_vec, O_CA_vec, O0_vec, H_vec)//, O_vec)
  {
    O_AC_vec = GetWorkSpaceThreadComplex(NCisAjs*NExcitation);
    O_CA_vec = GetWorkSpaceThreadComplex(NCisAjs*NExcitation);
    //O0_AC_vec = GetWorkSpaceThreadComplex(NCisAjs*NExcitation);
    O0_vec = GetWorkSpaceThreadComplex(NCisAjs*NExcitation);
    //H0_AC_vec = GetWorkSpaceThreadComplex(NCisAjs*NExcitation);
    H0_vec = GetWorkSpaceThreadComplex(NCisAjs*NExcitation);
    
    W2_vec = GetWorkSpaceThreadComplex(NCisAjs);
    one_over_W2_vec = GetWorkSpaceThreadComplex(NCisAjs);
    //H_vec = GetWorkSpaceThreadComplex(NExcitation);
    //O_vec = GetWorkSpaceThreadComplex(NExcitation);
    O_AC_corr_n = GetWorkSpaceThreadComplex(NExcitation);
    O_CA_corr_n = GetWorkSpaceThreadComplex(NExcitation);
    
    
    myEleIdx = GetWorkSpaceThreadInt(Nsize);
    myEleNum = GetWorkSpaceThreadInt(Nsite2);
    myProjCntNew = GetWorkSpaceThreadInt(NProj);
    myBuffer = GetWorkSpaceThreadComplex(NQPFull+2*Nsize);

    //#pragma loop noalias
    for(idx=0;idx<Nsize;idx++) myEleIdx[idx] = eleIdx[idx];
    //#pragma loop noalias
    for(idx=0;idx<Nsite2;idx++) myEleNum[idx] = eleNum[idx];

    
    double factor;
    int rm, rn, u;
    int idx_int, idx_trans;

//#pragma omp for private(idx,ri,rj,s,tmp) schedule(dynamic) nowait
    for(idx=0;idx<NCisAjs;idx++) {
      ri = CisAjsIdx[idx][0];
      rj = CisAjsIdx[idx][2];
      s  = CisAjsIdx[idx][3];
      tmp = GreenFunc1(ri,rj,s,ip,myEleIdx,eleCfg,myEleNum,eleProjCnt,
                       myProjCntNew,myBuffer);
      LocalCisAjs[idx] = tmp;
    }

//#pragma omp for private(idx) nowait
    for(idx=0;idx<NCisAjs;idx++) {
      PhysCisAjs[idx] += w*LocalCisAjs[idx];
    }

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//#pragma omp for private(idx,ri,rj,s,rk,rl,t,tmp) schedule(dynamic) nowait    
    for(idx=0;idx<NCisAjs;idx++) {
      ri = CisAjsIdx[idx][0];
      rj = CisAjsIdx[idx][2];
      s  = CisAjsIdx[idx][3];
      double complex tmp_CA =  1.*LocalCisAjs[idx];
      int idx_exc;
      for(idx_exc=0;idx_exc<NExcitation;idx_exc++){
        //int idx_vector = idx+idx_exc*NCisAjs;
        int idx_vector = idx_exc + idx*NExcitation;
        int idx_green = ijst_to_idx[rj+s*Nsite][ri+s*Nsite];
        int dr = ChargeExcitationIdx[idx_exc][1];
        int t  = ChargeExcitationIdx[idx_exc][0];
        int ra = (dr+ri+Nsite)%Nsite;
        int rb = (dr+rj+Nsite)%Nsite;
        
        
        // <phi|ac|x> / <phi|x> = delta_{ri,rj} * <phi|x> / <phi|x> - <phi|ca|x> / <phi|x>           <-- need to reverse indices
        O_AC_vec[idx_vector]  = del(ri,rj)      * ((double) (Commute_Nat_(with_nothing, ra, s, t,  0,  0, 0, myEleNum)));
        O_AC_vec[idx_vector] -= LocalCisAjs[idx_green]* ((double) (Commute_Nat_(with_CisAjs,  ra, s, t, rj, ri, s, myEleNum)));

        // <phi|ca|x> / <phi|x>
        O_CA_vec[idx_vector]  = LocalCisAjs[idx] * ((double) (Commute_Nat_(with_CisAjs,  ra, s, t, ri, rj, s, myEleNum)));        
        
        // <phi|x>
        O0_vec[idx_vector]  = 1.0 * ((double) (Commute_Nat_(with_nothing, rb, s, t,  0,  0, 0, myEleNum)));
        
        // <phi|H_U|x> 
        tmp=0.0;
      
        for(idx_int=0;idx_int<NCoulombIntra;idx_int++) {
          rm = CoulombIntra[idx_int];
          tmp += ParaCoulombIntra[idx_int]*myEleNum[rm]*myEleNum[rm+Nsite]; 
        }
        H0_vec[idx_vector] =  tmp * ((double) (Commute_Nat_(with_nothing, rb, s, t, 0, 0, 0, myEleNum))) ;
      
        // <phi|H_T|x> / <phi|x>
        for(idx_trans=0;idx_trans<NTransfer;idx_trans++) {
          
          rm = Transfer[idx_trans][0];
          rn = Transfer[idx_trans][2];
          u  = Transfer[idx_trans][3];
          idx_green = ijst_to_idx[rm+u*Nsite][rn+u*Nsite];
          
          H0_vec[idx_vector] -= ParaTransfer[idx_trans] * LocalCisAjs[idx_green] * ((double) (Commute_Nat_(with_CisAjs, rb, s, t, rm, rn, u, myEleNum))) ;
        }
      }
    }
    
    int nn,mm;
    double sqrt_w2 = 0.0, factor_w2 = 1.0;
    for (idx = 0; idx < NCisAjs; idx++) {
      //W2_vec[idx] = 0.0;
      //for (nn = 0; nn < NExcitation; nn++) {
      //  W2_vec[idx] += O0_vec[idx + nn*NCisAjs]*conj(O0_vec[idx + nn*NCisAjs]);
      //}
      one_over_W2_vec[idx] = 1.0;//ip/sqrt(W2_vec[idx]);
    }
    
    //sqrt_w2 = sqrt(w2);
    //factor_w2 = conj(ip)*conj(ip)/sqrt_w2;
    
    //printf("\n%d %d %d %d\n", myEleNum[0],myEleNum[1], myEleNum[2], myEleNum[3]);
    
    //*
    int ii;
    for (ii = 0; ii < NCisAjs; ii++) {
      ri = CisAjsIdx[ii][0];
      rj = CisAjsIdx[ii][2];
      s  = CisAjsIdx[ii][3];
      
      for (nn = 0; nn < NExcitation; nn++) {
        O_AC_corr_n[nn] = 0.0;
        O_CA_corr_n[nn] = 0.0;

        for(idx_int=0;idx_int<NCoulombIntra;idx_int++) {
          rm = CoulombIntra[idx_int];
          
          if(rm==rj) {
            O_AC_corr_n[nn] += ParaCoulombIntra[idx_int]*myEleNum[rm+(1-s)*Nsite] * O_AC_vec[nn + ii*NExcitation];
            O_CA_corr_n[nn] += ParaCoulombIntra[idx_int]*myEleNum[rm+(1-s)*Nsite] * O_CA_vec[nn + ii*NExcitation];
          }
        }
        
        for(idx_trans=0;idx_trans<NTransfer;idx_trans++) {
        
          rm = Transfer[idx_trans][0];
          rn = Transfer[idx_trans][2];
          u  = Transfer[idx_trans][3];
          if(rn==rj) {
            int idx_green = ijst_to_idx[ri+s*Nsite][rm+s*Nsite];
            O_AC_corr_n[nn] -= 0.5*ParaTransfer[idx_trans] * O_AC_vec[nn + idx_green*NExcitation] ;
            O_CA_corr_n[nn] -= 0.5*ParaTransfer[idx_trans] * O_CA_vec[nn + idx_green*NExcitation] ;
          }
        }
      }
      
      int N  = NExcitation;
      int N2 = NExcitation*NExcitation;
      double complex alpha = w;// * one_over_W2_vec[ii];
      
      C_ADD_AxB(&Phys_nACm[ii*N2],&O_AC_vec[ii*N], &O0_vec[ii*N], N, alpha);
      C_ADD_AxB(&Phys_nCAm[ii*N2],&O_CA_vec[ii*N], &O0_vec[ii*N], N, alpha);
      
      C_ADD_AxB(&Phys_nAHCm[ii*N2],&O_AC_vec[ii*N], &H0_vec[ii*N], N, alpha);
      C_ADD_AxB(&Phys_nAHCm[ii*N2],&O_AC_corr_n[0], &O0_vec[ii*N], N, alpha);

      C_ADD_AxB(&Phys_nCHAm[ii*N2],&O_CA_vec[ii*N], &H0_vec[ii*N], N, alpha);
      C_ADD_AxB(&Phys_nCHAm[ii*N2],&O_CA_corr_n[0], &O0_vec[ii*N], N,-alpha);
    }
    
    
    
    
  } //
  
  

  ReleaseWorkSpaceThreadInt();
  ReleaseWorkSpaceThreadComplex();
  return;
}
*/




void CalculateGreenFunc(const double w, const double complex ip, int *eleIdx, int *eleCfg,
                        int *eleNum, int *eleProjCnt) {

  int idx,idx0,idx1;
  int ri,rj,s,rk,rl,t;
  double complex tmp;
  int *myEleIdx, *myEleNum, *myProjCntNew;
  double complex *myBuffer;
  
  RequestWorkSpaceThreadInt(Nsize+Nsite2+NProj);
  RequestWorkSpaceThreadComplex(NQPFull+2*Nsize);
  /* GreenFunc1: NQPFull, GreenFunc2: NQPFull+2*Nsize */

  #pragma omp parallel default(shared)                \
  private(myEleIdx,myEleNum,myProjCntNew,myBuffer,idx)
  {
    myEleIdx = GetWorkSpaceThreadInt(Nsize);
    myEleNum = GetWorkSpaceThreadInt(Nsite2);
    myProjCntNew = GetWorkSpaceThreadInt(NProj);
    myBuffer = GetWorkSpaceThreadComplex(NQPFull+2*Nsize);

    #pragma loop noalias
    for(idx=0;idx<Nsize;idx++) myEleIdx[idx] = eleIdx[idx];
    #pragma loop noalias
    for(idx=0;idx<Nsite2;idx++) myEleNum[idx] = eleNum[idx];

    #pragma omp master
    {StartTimer(50);}

    #pragma omp for private(idx,ri,rj,s,tmp) schedule(dynamic) nowait
    for(idx=0;idx<NCisAjs;idx++) {
      ri = CisAjsIdx[idx][0];
      rj = CisAjsIdx[idx][2];
      s  = CisAjsIdx[idx][3];
      tmp = GreenFunc1(ri,rj,s,ip,myEleIdx,eleCfg,myEleNum,eleProjCnt,
                       myProjCntNew,myBuffer);
      LocalCisAjs[idx] = tmp;
    }
    #pragma omp master
    {StopTimer(50);StartTimer(51);}
    
    #pragma omp for private(idx,ri,rj,s,rk,rl,t,tmp) schedule(dynamic)
    for(idx=0;idx<NCisAjsCktAltDC;idx++) {
      ri = CisAjsCktAltDCIdx[idx][0];
      rj = CisAjsCktAltDCIdx[idx][2];
      s  = CisAjsCktAltDCIdx[idx][1];
      rk = CisAjsCktAltDCIdx[idx][4];
      rl = CisAjsCktAltDCIdx[idx][6];
      t  = CisAjsCktAltDCIdx[idx][5];

      tmp = GreenFunc2(ri,rj,rk,rl,s,t,ip,myEleIdx,eleCfg,myEleNum,eleProjCnt,
                       myProjCntNew,myBuffer);
      PhysCisAjsCktAltDC[idx] += w*tmp;
    }
    
    #pragma omp master
    {StopTimer(51);StartTimer(52);}

    #pragma omp for private(idx) nowait
    for(idx=0;idx<NCisAjs;idx++) {
      PhysCisAjs[idx] += w*LocalCisAjs[idx];
    }

    #pragma omp master
    {StopTimer(52);StartTimer(53);}

    #pragma omp for private(idx,idx0,idx1) nowait
    for(idx=0;idx<NCisAjsCktAlt;idx++) {
      idx0 = CisAjsCktAltIdx[idx][0];
      idx1 = CisAjsCktAltIdx[idx][1];
      PhysCisAjsCktAlt[idx] += w*LocalCisAjs[idx0]*conj(LocalCisAjs[idx1]);// TBC conj ok?
    }

    #pragma omp master
    {StopTimer(53);}
  }

  ReleaseWorkSpaceThreadInt();
  ReleaseWorkSpaceThreadComplex();
  return;
}


void CalculateGreenFuncBF(const double w, const double ip, int *eleIdx, int *eleCfg,
                          int *eleNum, int *eleProjCnt, const int *eleProjBFCnt) {

  int idx,idx0,idx1;
  int ri,rj,s,rk,rl,t;
  double complex tmp;
  int *myEleIdx, *myEleNum, *myEleCfg, *myProjCntNew, *myProjBFCntNew;
  double complex* mySltBFTmp;
  double complex* myBuffer;

  RequestWorkSpaceThreadInt(Nsize+2*Nsite2+NProj+16*Nsite*Nrange);
  RequestWorkSpaceThreadComplex(NQPFull+2*Nsize+NQPFull*Nsite2*Nsite2);
  /* GreenFunc1: NQPFull, GreenFunc2: NQPFull+2*Nsize */

#pragma omp parallel default(shared)\
  private(myEleIdx,myEleNum,myEleCfg,myProjCntNew,myProjBFCntNew,myBuffer,mySltBFTmp)
  {
    myEleIdx = GetWorkSpaceThreadInt(Nsize);
    myEleNum = GetWorkSpaceThreadInt(Nsite2);
    myEleCfg = GetWorkSpaceThreadInt(Nsite2);
    myProjCntNew   = GetWorkSpaceThreadInt(NProj);
    myProjBFCntNew = GetWorkSpaceThreadInt(16*Nsite*Nrange);
    myBuffer   = GetWorkSpaceThreadComplex(NQPFull+2*Nsize);
    mySltBFTmp = GetWorkSpaceThreadComplex(NQPFull*Nsite2*Nsite2);

#pragma loop noalias
    for(idx=0;idx<Nsize;idx++) myEleIdx[idx] = eleIdx[idx];
#pragma loop noalias
    for(idx=0;idx<Nsite2;idx++) myEleNum[idx] = eleNum[idx];
#pragma loop noalias
    for(idx=0;idx<Nsite2;idx++) myEleCfg[idx] = eleCfg[idx];

    StoreSlaterElmBF_fcmp(mySltBFTmp);
#pragma omp master
    {StartTimer(50);}

#pragma omp for private(idx,ri,rj,s,tmp) schedule(dynamic) nowait
    for(idx=0;idx<NCisAjs;idx++) {
      ri = CisAjsIdx[idx][0];
      rj = CisAjsIdx[idx][2];
      s  = CisAjsIdx[idx][3];
      tmp = GreenFunc1BF(ri,rj,s,ip,mySltBFTmp,myEleIdx,myEleCfg,myEleNum,eleProjCnt,myProjCntNew,eleProjBFCnt,myProjBFCntNew,myBuffer);
      LocalCisAjs[idx] = tmp;
    }

#pragma omp master
    {StopTimer(50);StartTimer(51);}

#pragma omp for private(idx,ri,rj,s,rk,rl,t,tmp) schedule(dynamic)
    for(idx=0;idx<NCisAjsCktAltDC;idx++) {
      ri = CisAjsCktAltDCIdx[idx][0];
      rj = CisAjsCktAltDCIdx[idx][2];
      s  = CisAjsCktAltDCIdx[idx][1];
      rk = CisAjsCktAltDCIdx[idx][4];
      rl = CisAjsCktAltDCIdx[idx][6];
      t  = CisAjsCktAltDCIdx[idx][5];
      tmp = GreenFunc2(ri,rj,rk,rl,s,t,ip,myEleIdx,eleCfg,myEleNum,eleProjCnt,
                       myProjCntNew,myBuffer);
      PhysCisAjsCktAltDC[idx] += w*tmp;
    }

#pragma omp master
    {StopTimer(51);StartTimer(52);}

#pragma omp for private(idx) nowait
    for(idx=0;idx<NCisAjs;idx++) {
      PhysCisAjs[idx] += w*LocalCisAjs[idx];
    }

#pragma omp master
    {StopTimer(52);StartTimer(53);}

#pragma omp for private(idx,idx0,idx1) nowait
    for(idx=0;idx<NCisAjsCktAlt;idx++) {
      idx0 = CisAjsCktAltIdx[idx][0];
      idx1 = CisAjsCktAltIdx[idx][1];
      PhysCisAjsCktAlt[idx] += w*LocalCisAjs[idx0]*LocalCisAjs[idx1];
    }

#pragma omp master
    {StopTimer(53);}
  }

  ReleaseWorkSpaceThreadInt();
  ReleaseWorkSpaceThreadComplex();
  return;
}
#endif
