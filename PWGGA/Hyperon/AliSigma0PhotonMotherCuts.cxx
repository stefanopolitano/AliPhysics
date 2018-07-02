#include "AliSigma0PhotonMotherCuts.h"
#include <iostream>

ClassImp(AliSigma0PhotonMotherCuts)

    //____________________________________________________________________________________________________
    AliSigma0PhotonMotherCuts::AliSigma0PhotonMotherCuts()
    : TObject(),
      fHistograms(nullptr),
      fHistogramsMC(nullptr),
      fIsMC(false),
      fIsLightweight(false),
      fInputEvent(nullptr),
      fMCEvent(nullptr),
      fLambdaMixed(),
      fPhotonMixed(),
      fMixingDepth(10),
      fPDG(0),
      fPDGDaughter1(0),
      fPDGDaughter2(0),
      fMassSigma(0),
      fSigmaMassCut(0),
      fSigmaSidebandLow(0),
      fSigmaSidebandUp(0),
      fArmenterosCut(true),
      fArmenterosQtLow(0.f),
      fArmenterosQtUp(0.f),
      fArmenterosAlphaLow(0.f),
      fArmenterosAlphaUp(0.f),
      fHistCutBooking(nullptr),
      fHistNSigma(nullptr),
      fHistPt(nullptr),
      fHistMassCutPt(nullptr),
      fHistInvMass(nullptr),
      fHistInvMassBeforeArmenteros(nullptr),
      fHistInvMassRec(nullptr),
      fHistInvMassPt(nullptr),
      fHistInvMassEta(nullptr),
      fHistEtaPhi(nullptr),
      fHistRapidity(nullptr),
      fHistPtY(),
      fHistArmenterosBefore(nullptr),
      fHistArmenterosAfter(nullptr),
      fHistMixedPt(nullptr),
      fHistMixedInvMass(nullptr),
      fHistMixedPtY(),
      fHistMixedInvMassPt(nullptr),
      fHistMixedInvMassEta(nullptr),
      fHistMCTruthPt(nullptr),
      fHistMCTruthPtY(nullptr),
      fHistMCTruthPtEta(nullptr),
      fHistMCTruthDaughterPt(nullptr),
      fHistMCTruthDaughterPtY(nullptr),
      fHistMCTruthDaughterPtEta(nullptr),
      fHistMCV0Pt(nullptr),
      fHistMCV0Mass(nullptr) {}

//____________________________________________________________________________________________________
AliSigma0PhotonMotherCuts::AliSigma0PhotonMotherCuts(
    const AliSigma0PhotonMotherCuts &ref)
    : TObject(ref),
      fHistograms(nullptr),
      fHistogramsMC(nullptr),
      fIsMC(false),
      fIsLightweight(false),
      fInputEvent(nullptr),
      fMCEvent(nullptr),
      fLambdaMixed(),
      fPhotonMixed(),
      fMixingDepth(10),
      fPDG(0),
      fPDGDaughter1(0),
      fPDGDaughter2(0),
      fMassSigma(0),
      fSigmaMassCut(0),
      fSigmaSidebandLow(0),
      fSigmaSidebandUp(0),
      fArmenterosCut(true),
      fArmenterosQtLow(0.f),
      fArmenterosQtUp(0.f),
      fArmenterosAlphaLow(0.f),
      fArmenterosAlphaUp(0.f),
      fHistCutBooking(nullptr),
      fHistNSigma(nullptr),
      fHistPt(nullptr),
      fHistMassCutPt(nullptr),
      fHistInvMass(nullptr),
      fHistInvMassBeforeArmenteros(nullptr),
      fHistInvMassRec(nullptr),
      fHistInvMassPt(nullptr),
      fHistInvMassEta(nullptr),
      fHistEtaPhi(nullptr),
      fHistRapidity(nullptr),
      fHistPtY(),
      fHistArmenterosBefore(nullptr),
      fHistArmenterosAfter(nullptr),
      fHistMixedPt(nullptr),
      fHistMixedInvMass(nullptr),
      fHistMixedPtY(),
      fHistMixedInvMassPt(nullptr),
      fHistMixedInvMassEta(nullptr),
      fHistMCTruthPt(nullptr),
      fHistMCTruthPtY(nullptr),
      fHistMCTruthPtEta(nullptr),
      fHistMCTruthDaughterPt(nullptr),
      fHistMCTruthDaughterPtY(nullptr),
      fHistMCTruthDaughterPtEta(nullptr),
      fHistMCV0Pt(nullptr),
      fHistMCV0Mass(nullptr) {}

//____________________________________________________________________________________________________
AliSigma0PhotonMotherCuts &AliSigma0PhotonMotherCuts::operator=(
    const AliSigma0PhotonMotherCuts &ref) {
  // Assignment operator
  if (this == &ref) return *this;

  return (*this);
}

//____________________________________________________________________________________________________
AliSigma0PhotonMotherCuts *AliSigma0PhotonMotherCuts::DefaultCuts() {
  AliSigma0PhotonMotherCuts *photonMotherCuts = new AliSigma0PhotonMotherCuts();
  photonMotherCuts->SetArmenterosCut(0., 0.12, -1., -0.6);
  return photonMotherCuts;
}

//____________________________________________________________________________________________________
void AliSigma0PhotonMotherCuts::SelectPhotonMother(
    AliVEvent *inputEvent, AliMCEvent *mcEvent,
    const std::vector<AliSigma0ParticleV0> &photonCandidates,
    const std::vector<AliSigma0ParticleV0> &lambdaCandidates) {
  fMCEvent = mcEvent;

  if (fIsMC) {
    ProcessMC();
  }

  fInputEvent = inputEvent;

  // Particle pairing
  SigmaToLambdaGamma(photonCandidates, lambdaCandidates);
  SigmaToLambdaGammaMixedEvent(photonCandidates, lambdaCandidates);
  FillEventBuffer(photonCandidates, lambdaCandidates);
}

//____________________________________________________________________________________________________
void AliSigma0PhotonMotherCuts::SigmaToLambdaGamma(
    const std::vector<AliSigma0ParticleV0> &photonCandidates,
    const std::vector<AliSigma0ParticleV0> &lambdaCandidates) {
  // SAME EVENT
  for (const auto &photon : photonCandidates) {
    int nSigma = 0;
    for (const auto &lambda : lambdaCandidates) {
      /// Candidates with lambdas with shared daughter tracks are not used
      if (!lambda.GetIsUse()) continue;
      const AliSigma0ParticlePhotonMother sigma(lambda, photon, fInputEvent);
      const float invMass = sigma.GetMass();
      const float armAlpha = sigma.GetArmenterosAlpha();
      const float armQt = sigma.GetArmenterosQt();
      if (!fIsLightweight) {
        fHistArmenterosBefore->Fill(armAlpha, armQt);
        fHistInvMassBeforeArmenteros->Fill(invMass);
      }
      // Armenteros cut
      if (fArmenterosCut) {
        if (armQt > fArmenterosQtUp || armQt < fArmenterosQtLow) continue;
        if (armAlpha > fArmenterosAlphaUp || armAlpha < fArmenterosAlphaLow)
          continue;
      }
      const float rap = sigma.GetRapidity();
      const int rapBin = GetRapidityBin(rap);
      if (!fIsLightweight) {
        fHistArmenterosAfter->Fill(armAlpha, armQt);
        fHistPt->Fill(sigma.GetPt());
        fHistInvMass->Fill(invMass);
        fHistInvMassRec->Fill(sigma.GetRecMass());
        fHistRapidity->Fill(rap);
        if (rapBin > -1) fHistPtY[rapBin]->Fill(sigma.GetPt(), invMass);
        fHistInvMassEta->Fill(sigma.GetEta(), invMass);
      }
      fHistInvMassPt->Fill(sigma.GetPt(), invMass);
      if (invMass < fMassSigma + fSigmaMassCut &&
          invMass > fMassSigma - fSigmaMassCut) {
        if (!fIsLightweight) {
          fHistMassCutPt->Fill(sigma.GetPt());
          fHistEtaPhi->Fill(sigma.GetEta(), sigma.GetPhi());
        }
        ++nSigma;
        if (fIsMC) {
          const int label =
              sigma.MatchToMC(fMCEvent, fPDG, {{fPDGDaughter1, fPDGDaughter2}});
          if (label > 0) {
            fHistMCV0Pt->Fill(sigma.GetPt());
            fHistMCV0Mass->Fill(invMass);
          }
        }
      }
    }
    if (!fIsLightweight) fHistNSigma->Fill(nSigma);
  }
}

//____________________________________________________________________________________________________
void AliSigma0PhotonMotherCuts::SigmaToLambdaGammaMixedEvent(
    const std::vector<AliSigma0ParticleV0> &photonCandidates,
    const std::vector<AliSigma0ParticleV0> &lambdaCandidates) {
  // photons from this event with mixed lambdas
  for (const auto &LambdaContainer : fLambdaMixed) {
    for (const auto &Lambda : LambdaContainer) {
      if (!Lambda.GetIsUse()) continue;
      for (auto Photon : photonCandidates) {
        const AliSigma0ParticlePhotonMother sigma(Lambda, Photon, fInputEvent);
        // Armenteros cut
        const float armAlpha = sigma.GetArmenterosAlpha();
        const float armQt = sigma.GetArmenterosQt();
        if (fArmenterosCut) {
          if (armQt > fArmenterosQtUp || armQt < fArmenterosQtLow) continue;
          if (armAlpha > fArmenterosAlphaUp || armAlpha < fArmenterosAlphaLow)
            continue;
        }
        const float invMass = sigma.GetMass();
        fHistMixedInvMassPt->Fill(sigma.GetPt(), invMass);
        if (!fIsLightweight) {
          fHistMixedPt->Fill(sigma.GetPt());
          fHistMixedInvMass->Fill(invMass);
          const float rap = sigma.GetRapidity();
          const int rapBin = GetRapidityBin(rap);
          if (rapBin > -1) fHistMixedPtY[rapBin]->Fill(sigma.GetPt(), invMass);
          fHistMixedInvMassEta->Fill(sigma.GetEta(), invMass);
        }
      }
    }
  }

  // lambdas from this event with mixed photons
  for (const auto &PhotonContainer : fPhotonMixed) {
    for (const auto &Photon : PhotonContainer) {
      for (const auto &Lambda : lambdaCandidates) {
        if (!Lambda.GetIsUse()) continue;
        const AliSigma0ParticlePhotonMother sigma(Lambda, Photon, fInputEvent);
        // Armenteros cut
        const float armAlpha = sigma.GetArmenterosAlpha();
        const float armQt = sigma.GetArmenterosQt();
        if (fArmenterosCut) {
          if (armQt > fArmenterosQtUp || armQt < fArmenterosQtLow) continue;
          if (armAlpha > fArmenterosAlphaUp || armAlpha < fArmenterosAlphaLow)
            continue;
        }
        const float invMass = sigma.GetMass();
        fHistMixedInvMassPt->Fill(sigma.GetPt(), invMass);
        if (!fIsLightweight) {
          fHistMixedPt->Fill(sigma.GetPt());
          fHistMixedInvMass->Fill(invMass);
          const float rap = sigma.GetRapidity();
          const int rapBin = GetRapidityBin(rap);
          if (rapBin > -1) fHistMixedPtY[rapBin]->Fill(sigma.GetPt(), invMass);
          fHistMixedInvMassEta->Fill(sigma.GetEta(), invMass);
        }
      }
    }
  }
}

//____________________________________________________________________________________________________
void AliSigma0PhotonMotherCuts::FillEventBuffer(
    const std::vector<AliSigma0ParticleV0> &photonCandidates,
    const std::vector<AliSigma0ParticleV0> &lambdaCandidates) {
  // ++++++++++++++
  // Photon
  if (static_cast<int>(photonCandidates.size()) > 0) {
    if (static_cast<int>(fPhotonMixed.size()) < fMixingDepth) {
      fPhotonMixed.push_back(photonCandidates);
    } else {
      fPhotonMixed.pop_front();
      fPhotonMixed.push_back(photonCandidates);
    }
  }

  // ++++++++++++++
  // Lambda
  if (static_cast<int>(lambdaCandidates.size()) > 0) {
    if (static_cast<int>(fLambdaMixed.size()) < fMixingDepth) {
      fLambdaMixed.push_back(lambdaCandidates);
    } else {
      fLambdaMixed.pop_front();
      fLambdaMixed.push_back(lambdaCandidates);
    }
  }
}

//____________________________________________________________________________________________________
float AliSigma0PhotonMotherCuts::ComputeRapidity(float pt, float pz,
                                                 float m) const {
  // calculates rapidity keeping the sign in case E == pz

  float energy = std::sqrt(pt * pt + pz * pz + m * m);
  if (energy != std::fabs(pz))
    return 0.5 * std::log((energy + pz) / (energy - pz));
  return (pz >= 0) ? 1.e30 : -1.e30;
}

//____________________________________________________________________________________________________
void AliSigma0PhotonMotherCuts::ProcessMC() const {
  // Loop over the MC tracks
  for (int iPart = 1; iPart < (fMCEvent->GetNumberOfTracks()); iPart++) {
    AliMCParticle *mcParticle =
        static_cast<AliMCParticle *>(fMCEvent->GetTrack(iPart));
    if (!mcParticle) continue;
    //    if (!mcParticle->IsPhysicalPrimary()) continue;
    if (mcParticle->GetNDaughters() != 2) continue;
    if (mcParticle->PdgCode() != fPDG) continue;
    fHistMCTruthPt->Fill(mcParticle->Pt());
    fHistMCTruthPtY->Fill(
        ComputeRapidity(mcParticle->Pt(), mcParticle->Pz(), mcParticle->M()),
        mcParticle->Pt());
    fHistMCTruthPtEta->Fill(mcParticle->Eta(), mcParticle->Pt());

    if (!CheckDaughters(mcParticle)) continue;
    fHistMCTruthDaughterPt->Fill(mcParticle->Pt());
    fHistMCTruthDaughterPtY->Fill(
        ComputeRapidity(mcParticle->Pt(), mcParticle->Pz(), mcParticle->M()),
        mcParticle->Pt());
    fHistMCTruthDaughterPtEta->Fill(mcParticle->Eta(), mcParticle->Pt());
  }
}

//____________________________________________________________________________________________________
bool AliSigma0PhotonMotherCuts::CheckDaughters(
    const AliMCParticle *particle) const {
  AliMCParticle *posDaughter = nullptr;
  AliMCParticle *negDaughter = nullptr;

  if (particle->GetNDaughters() != 2) return false;

  for (int daughterIndex = particle->GetFirstDaughter();
       daughterIndex <= particle->GetLastDaughter(); ++daughterIndex) {
    if (daughterIndex < 0) continue;
    AliMCParticle *tmpDaughter =
        static_cast<AliMCParticle *>(fMCEvent->GetTrack(daughterIndex));
    if (!tmpDaughter) continue;
    const int pdgCode = tmpDaughter->PdgCode();
    if (pdgCode == fPDGDaughter1)
      posDaughter = tmpDaughter;
    else if (pdgCode == fPDGDaughter2)
      negDaughter = tmpDaughter;
  }

  if (!posDaughter || !negDaughter) return false;

  return true;
}

//____________________________________________________________________________________________________
int AliSigma0PhotonMotherCuts::GetRapidityBin(float rapidity) const {
  if (rapidity <= -1.0)
    return 0;
  else if (-1.0 < rapidity && rapidity <= -0.9)
    return 1;
  else if (-0.9 < rapidity && rapidity <= -0.8)
    return 2;
  else if (-0.8 < rapidity && rapidity <= -0.7)
    return 3;
  else if (-0.7 < rapidity && rapidity <= -0.6)
    return 4;
  else if (-0.6 < rapidity && rapidity <= -0.5)
    return 5;
  else if (-0.5 < rapidity && rapidity <= -0.4)
    return 6;
  else if (-0.4 < rapidity && rapidity <= -0.3)
    return 7;
  else if (-0.3 < rapidity && rapidity <= -0.2)
    return 8;
  else if (-0.2 < rapidity && rapidity <= -0.1)
    return 9;
  else if (-0.1 < rapidity && rapidity <= 0.f)
    return 10;
  else if (0.f < rapidity && rapidity <= 0.1)
    return 11;
  else if (0.1 < rapidity && rapidity <= 0.2)
    return 12;
  else if (0.2 < rapidity && rapidity <= 0.3)
    return 13;
  else if (0.3 < rapidity && rapidity <= 0.4)
    return 14;
  else if (0.4 < rapidity && rapidity <= 0.5)
    return 15;
  else if (0.5 < rapidity && rapidity <= 0.6)
    return 16;
  else if (0.6 < rapidity && rapidity <= 0.7)
    return 17;
  else if (0.7 < rapidity && rapidity <= 0.8)
    return 18;
  else if (0.8 < rapidity && rapidity <= 0.9)
    return 19;
  else if (0.9 < rapidity && rapidity <= 1.f)
    return 20;
  else if (1.0 < rapidity)
    return 21;
  else
    return -1;
}

//____________________________________________________________________________________________________
void AliSigma0PhotonMotherCuts::InitCutHistograms(TString appendix) {
  std::cout << "============================\n"
            << " PHOTON MOTHER CUT CONFIGURATION \n"
            << " Sigma0 mass     " << fMassSigma << "\n"
            << " Sigma0 select   " << fSigmaMassCut << "\n"
            << " Sigma0 sideb    " << fSigmaSidebandLow << " "
            << fSigmaSidebandUp << "\n"
            << " Mixing depth    " << fMixingDepth << "\n"
            << "============================\n";

  std::vector<float> rapBins = {
      {-10.f, -1.f, -0.9, -0.8, -0.7, -0.6, -0.5, -0.4, -0.3, -0.2, -0.1, 0.f,
       0.1,   0.2,  0.3,  0.4,  0.5,  0.6,  0.7,  0.8,  0.9,  1.f,  10.f}};

  TH1::AddDirectory(kFALSE);

  TString name;
  if (fHistograms != nullptr) {
    delete fHistograms;
    fHistograms = nullptr;
  }
  if (fHistograms == nullptr) {
    fHistograms = new TList();
    fHistograms->SetOwner(kTRUE);
    fHistograms->SetName(appendix);
  }

  fHistCutBooking = new TProfile("fHistCutBooking", ";;Cut value", 10, 0, 10);
  fHistCutBooking->GetXaxis()->SetBinLabel(1, "#Sigma^{0} selection");
  fHistCutBooking->GetXaxis()->SetBinLabel(2, "#Sigma^{0} sideband low");
  fHistCutBooking->GetXaxis()->SetBinLabel(3, "#Sigma^{0} sideband up");
  fHistCutBooking->GetXaxis()->SetBinLabel(4, "#Sigma^{0} mixing depth");
  fHistCutBooking->GetXaxis()->SetBinLabel(5, "Armenteros q_{T} low");
  fHistCutBooking->GetXaxis()->SetBinLabel(6, "Armenteros q_{T} up");
  fHistCutBooking->GetXaxis()->SetBinLabel(7, "Armenteros #alpha low");
  fHistCutBooking->GetXaxis()->SetBinLabel(8, "Armenteros #alpha up");
  fHistograms->Add(fHistCutBooking);

  fHistCutBooking->Fill(0.f, fSigmaMassCut);
  fHistCutBooking->Fill(1.f, fSigmaSidebandLow);
  fHistCutBooking->Fill(2.f, fSigmaSidebandUp);
  fHistCutBooking->Fill(3.f, fMixingDepth);
  fHistCutBooking->Fill(4.f, fArmenterosQtLow);
  fHistCutBooking->Fill(5.f, fArmenterosQtUp);
  fHistCutBooking->Fill(6.f, fArmenterosAlphaLow);
  fHistCutBooking->Fill(7.f, fArmenterosAlphaUp);

  fHistInvMassPt = new TH2F("fHistInvMassPt",
                            "; #it{p}_{T} #Lambda#gamma (GeV/#it{c}); "
                            "M_{#Lambda#gamma} (GeV/#it{c}^{2})",
                            1000, 0, 20, 2000, 1., 2.);
  fHistograms->Add(fHistInvMassPt);
  fHistMixedInvMassPt = new TH2F("fHistMixedInvMassPt",
                                 "; #it{p}_{T} #Lambda#gamma (GeV/#it{c}); "
                                 "M_{#Lambda#gamma} (GeV/#it{c}^{2})",
                                 1000, 0, 20, 2000, 1., 2.);
  fHistograms->Add(fHistMixedInvMassPt);

  if (!fIsLightweight) {
    fHistNSigma =
        new TH1F("fHistNSigma", ";# #Sigma candidates; Entries", 20, 0, 20);
    fHistPt =
        new TH1F("fHistPt", "; #it{p}_{T} #Lambda#gamma (GeV/#it{c}); Entries",
                 500, 0, 20);
    fHistMassCutPt = new TH1F(
        "fHistMassCutPt", "; #it{p}_{T} #Lambda#gamma (GeV/#it{c}); Entries",
        500, 0, 20);
    fHistInvMass =
        new TH1F("fHistInvMass",
                 "; M_{#Lambda#gamma} (GeV/#it{c}^{2}); Entries", 2000, 1., 2.);
    fHistInvMassBeforeArmenteros =
        new TH1F("fHistInvMassBeforeArmenteros",
                 "; M_{#Lambda#gamma} (GeV/#it{c}^{2}); Entries", 2000, 1., 2.);
    fHistInvMassRec =
        new TH1F("fHistInvMassRec",
                 "; M_{#Lambda#gamma} (GeV/#it{c}^{2}); Entries", 2000, 1., 2.);
    fHistInvMassEta = new TH2F("fHistInvMassEta",
                               "; #eta; M_{#Lambda#gamma} (GeV/#it{c}^{2})",
                               1000, 0, 1, 2000, 1., 2.);
    fHistArmenterosBefore =
        new TH2F("fHistArmenterosBefore", " ; #alpha; #it{q}_{T} (GeV/#it{c})",
                 500, -1, 1, 500, 0, 1);
    fHistArmenterosAfter =
        new TH2F("fHistArmenterosAfter", " ; #alpha; #it{q}_{T} (GeV/#it{c})",
                 500, -1, 1, 500, 0, 1);
    fHistEtaPhi = new TH2F("fHistEtaPhi", "; #eta; #phi", 200, -1, 1, 200, 0,
                           2 * TMath::Pi());
    fHistRapidity = new TH1F("fHistRapidity", "; y; Entries", 200, -1, 1);
    fHistMixedPt = new TH1F("fHistMixedPt",
                            "; #it{p}_{T} #Lambda#gamma (GeV/#it{c}); Entries",
                            500, 0, 20);
    fHistMixedInvMass =
        new TH1F("fHistMixedInvMass",
                 "; M_{#Lambda#gamma} (GeV/#it{c}^{2}); Entries", 2000, 1., 2.);
    fHistMixedInvMassEta = new TH2F(
        "fHistMixedInvMassEta", "; #eta; M_{#Lambda#gamma} (GeV/#it{c}^{2})",
        1000, 0, 1, 2000, 1., 2.);

    fHistograms->Add(fHistNSigma);
    fHistograms->Add(fHistPt);
    fHistograms->Add(fHistMassCutPt);
    fHistograms->Add(fHistInvMass);
    fHistograms->Add(fHistInvMassBeforeArmenteros);
    fHistograms->Add(fHistInvMassRec);
    fHistograms->Add(fHistInvMassEta);
    fHistograms->Add(fHistEtaPhi);
    fHistograms->Add(fHistRapidity);
    fHistograms->Add(fHistArmenterosBefore);
    fHistograms->Add(fHistArmenterosAfter);
    fHistograms->Add(fHistMixedPt);
    fHistograms->Add(fHistMixedInvMass);
    fHistograms->Add(fHistMixedInvMassEta);

    for (int i = 0; i < static_cast<int>(rapBins.size() - 1); ++i) {
      fHistPtY[i] =
          new TH2F(Form("fHistPtY_%.2f_%.2f", rapBins[i], rapBins[i + 1]),
                   Form("%.2f < y < %.2f ; #it{p}_{T} (GeV/#it{c}); "
                        "M_{#Lambda#gamma} (GeV/#it{c}^{2})",
                        rapBins[i], rapBins[i + 1]),
                   500, 0, 10, 1000, 1., 1.5);
      fHistMixedPtY[i] =
          new TH2F(Form("fHistMixedPtY_%.2f_%.2f", rapBins[i], rapBins[i + 1]),
                   Form("%.2f < y < %.2f ; #it{p}_{T} (GeV/#it{c}); "
                        "M_{#Lambda#gamma} (GeV/#it{c}^{2})",
                        rapBins[i], rapBins[i + 1]),
                   500, 0, 10, 1000, 1., 1.5);
      fHistograms->Add(fHistPtY[i]);
      fHistograms->Add(fHistMixedPtY[i]);
    }
  }

  if (fIsMC) {
    if (fHistogramsMC != nullptr) {
      delete fHistogramsMC;
      fHistogramsMC = nullptr;
    }
    if (fHistogramsMC == nullptr) {
      fHistogramsMC = new TList();
      fHistogramsMC->SetOwner(kTRUE);
      fHistogramsMC->SetName("MC");
    }

    fHistMCTruthPt = new TH1F("fHistMCTruthPt",
                              "; #it{p}_{T} (GeV/#it{c}); Entries", 500, 0, 10);
    fHistMCTruthPtY =
        new TH2F("fHistMCTruthPtY", "; y; #it{p}_{T} (GeV/#it{c})", 1000, -10,
                 10, 500, 0, 10);
    fHistMCTruthPtEta =
        new TH2F("fHistMCTruthPtEta", "; #eta; #it{p}_{T} (GeV/#it{c})", 500,
                 -10, 10, 500, 0, 10);
    fHistMCTruthDaughterPt =
        new TH1F("fHistMCTruthDaughterPt", "; #it{p}_{T} (GeV/#it{c}); Entries",
                 500, 0, 10);
    fHistMCTruthDaughterPtY =
        new TH2F("fHistMCTruthDaughterPtY", "; y; #it{p}_{T} (GeV/#it{c})",
                 1000, -10, 10, 500, 0, 10);
    fHistMCTruthDaughterPtEta =
        new TH2F("fHistMCTruthDaughterPtEta", "; #eta; #it{p}_{T} (GeV/#it{c})",
                 500, -10, 10, 500, 0, 10);
    fHistMCV0Pt = new TH1F("fHistMCV0Pt",
                           "; #it{p}_{T} #Lambda#gamma (GeV/#it{c}); Entries",
                           500, 0, 20);
    fHistMCV0Mass =
        new TH1F("fHistMCV0Mass",
                 "; M_{#Lambda#gamma} (GeV/#it{c}^{2}); Entries", 2000, 1., 2.);

    fHistogramsMC->Add(fHistMCTruthPt);
    fHistogramsMC->Add(fHistMCTruthPtY);
    fHistogramsMC->Add(fHistMCTruthPtEta);
    fHistogramsMC->Add(fHistMCTruthDaughterPt);
    fHistogramsMC->Add(fHistMCTruthDaughterPtY);
    fHistogramsMC->Add(fHistMCTruthDaughterPtEta);
    fHistogramsMC->Add(fHistMCV0Pt);
    fHistogramsMC->Add(fHistMCV0Mass);

    fHistograms->Add(fHistogramsMC);
  }
}
