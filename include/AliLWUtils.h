#ifndef AliLWUtils__h
#define AliLWUtils__h
#include <TObject.h>


/*
This file contains class definitions which were given to me by my 
supervisor to be able to read in the data in the data stored in ROOT-trees.
The original unmodified versions are stored in the folder 'raw'
as I had to modify some syntax in some places to get things to work.

*/

class AliLWTPCTrack : public TObject {
  public:
    AliLWTPCTrack();
    AliLWTPCTrack(Float_t pt, Float_t phi, Float_t eta, Short_t trFlag=1);
    ~AliLWTPCTrack();
    Bool_t IsEqual(TObject* obj) const;
    Bool_t IsSortable() const;
    Int_t Compare(TObject *obj) const;
    Float_t fPt;
    Float_t fPhi;
    Float_t fEta;
    Short_t fTrFlag;
  ClassDef(AliLWTPCTrack, 1);
};
class AliLWFMDTrack : public TObject {
  public:
    AliLWFMDTrack();
    AliLWFMDTrack(Float_t phi, Float_t eta, Short_t mult);
    ~AliLWFMDTrack();
    Float_t fPhi;
    Float_t fEta;
    Short_t fMult;
  ClassDef(AliLWFMDTrack, 1);
};
class AliLWEvent : public TObject {
  public:
    AliLWEvent();
    ~AliLWEvent();
    AliLWEvent(UInt_t runNo, Float_t vz, Float_t cent, Short_t evFlag=1);
    void Setup(UInt_t runNo, Float_t vz, Float_t cent, Short_t evFlag=1);
    UInt_t fRunNo;
    Float_t fVz;
    Float_t fCent;
    Short_t fEvFlag;
  ClassDef(AliLWEvent,1);
};
#endif
