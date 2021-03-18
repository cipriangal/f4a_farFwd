
void beamPipeRP_test()
{
  // Load basic libraries;
  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");

  EicGeoParData *beamPipe = new EicGeoParData("BEAMPIPERP", 0, 0);
  beamPipe->SetFileName("beampiperp.root");

  // Do not care much about precise description;

  //Alex's RP beam pipe
    
  Double_t length[5];
  Double_t inRadius1[5];
  Double_t outRadius1[5];
  Double_t inRadius2[5];
  Double_t outRadius2[5];
  Double_t rotationAngle[5];
  Double_t xCenter[5];
  Double_t yCenter[5];
  Double_t zCenter[5];

  Double_t coneLength[5];
  Double_t inRadiusIN[5]; //inner radius input side
  Double_t outRadiusIN[5]; //inner radius oupput side
  Double_t inRadiusOUT[5]; //outer radius input side
  Double_t outRadiusOUT[5]; //outer radius output side

  //Double_t coneLength[0]        = 1900; //17500.0;//16500; //23212.8;
  //Double_t inRadiusIN[0]        = 130.0; //168.0; //inner radius
  //Double_t inRadiusOUT[0]       = 132.0; //170.0; //outer radius
  //Double_t outRadiusIN[0]       = 150.0; //50.0;
  //Double_t outRadiusOUT[0]      = 152.0; //51.0;
  //Double_t xCenter[0]           = 750.0;  //1053.07;
  //Double_t yCenter[0]           = 0.0;
  //Double_t zCenter[0]           = 24000; //30988.5; //30315.0;

  length[0]            = 7400.0;//nominal = 8500 //17500.0;//16500; //23212.8;
  inRadius1[0]          = 145.0; //168.0; //inner radius
  outRadius1[0]         = 147.0; //170.0; //outer radius
  inRadius2[0]          = 145.0; //168.0; //inner radius
  outRadius2[0]         = 147.0; //170.0; //outer radius
  xCenter[0]           = 850.0;//nominal = 830.0
  yCenter[0]           = 0.0;
  zCenter[0]           = 26000; //nominal = 25500 //24000; //30315.0;

	
  //CDR
  length[1]            = 11500; //17500.0;//16500; //23212.8;
  inRadius1[1]          = 145.0; //168.0; //inner radius
  outRadius1[1]         = 147.0; //170.0; //outer radius
  inRadius2[1]          = 70.0; //168.0; //inner radius
  outRadius2[1]         = 72.0; //170.0; //outer radius
  xCenter[1]           = 1300.0; //nominal = 1300.0
  yCenter[1]           = 0.0;
  zCenter[1]           = 35500; //CDR


  //Wan
  //length[1]            = 3000; //17500.0;//16500; //23212.8;
  //inRadius[1]          = 150.0; //168.0; //inner radius
  //outRadius[1]         = 152.0; //170.0; //outer radius
  //xCenter[1]           = 900.0;
  //yCenter[1]           = 0.0;
  //zCenter[1]           = 27495.5; //Wan
  //
	

    
  Int_t numPieces = 2;
   
  TGeoRotation * rw[5]; 

  TGeoRotation *rw[0] = new TGeoRotation();
  double angle = 2.7; //0.035 * TMath::RadToDeg();

  rw[0]->RotateY(angle);

  TGeoRotation *rw[1] = new TGeoRotation();
  angle = 2.7; //0.028 * TMath::RadToDeg;

  rw[1]->RotateY(angle);

  for(int i = 0; i < numPieces; i++){
    
    TGeoCone *pipe = new TGeoCone(Form("RPBeamPipe%d",i), 0.1 * length[i]/2, 0.1*inRadius1[i], 0.1 * outRadius1[i],0.1*inRadius2[i], 0.1 * outRadius2[i]);
    //TGeoVolume *vpipe = new TGeoVolume(Form("RPBeamPipe%d",i), pipe, beamPipe->GetMedium("beryllium"));
    //TGeoVolume *vpipe = new TGeoVolume(Form("RPBeamPipe%d",i), pipe, beamPipe->GetMedium("aluminum"));       
    TGeoVolume *vpipe = new TGeoVolume(Form("RPBeamPipe%d",i), pipe, beamPipe->GetMedium("StainlessSteel"));
 
    EicGeoMap *fgmap = beamPipe->CreateNewMap();
    fgmap->AddGeantVolumeLevel(Form("RPBeamPipe%d",i),numPieces);
    fgmap->SetSingleSensorContainerVolume(Form("RPBeamPipe%d",i));
        
    beamPipe->AddLogicalVolumeGroup(0, 0, numPieces);//waferNum);
        
    UInt_t geant[1] = {0}, group = i, logical[3] = {0, 0, 0};
        
    if (beamPipe->SetMappingTableEntry(fgmap, geant, group, logical)) {
      cout << "Failed to set mapping table entry!" << endl;
      exit(0);
    } //if
        
    beamPipe->GetTopVolume()->AddNode(vpipe, 0, new TGeoCombiTrans(0.1 * xCenter[i],0.1 *  yCenter[i], 0.1 * zCenter[i], rw[i]));
  }

  beamPipe->GetColorTable()->AddPatternMatch       ("RPBeamPipe", kGray+2);
  beamPipe->GetTransparencyTable()->AddPatternMatch("RPBeamPipe", 60);

  // A unified user call which places assembled detector volume in a proper place in MASTER (top)
  // coordinate system, puts this MASTER (top) volume into GEANT volume tree, and dumps this tree
  // together with EicRoot mapping table in one file;
  beamPipe->FinalizeOutput();

  // Yes, always exit;
  exit(0);
} // beamPipeRP

