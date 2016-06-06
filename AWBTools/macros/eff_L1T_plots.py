#! /usr/bin/env python

###########################################
# Plot Maker for quick plots
#
# by David Curry
#
# 7.22.1014
###########################################

import sys
import os
import re
from ROOT import *
from matplotlib import interactive
from ROOT import gROOT
from eff_modules import *


title = '_runs_272750_273020'

file = '_req_L1_dEta'

# Root file of Histograms
file16 = TFile('plots/L1T_analysis_singleMu16'+file+'.root')
# file0 = TFile('plots/L1T_analysis_singleMu0'+file+'.root')
# file5 = TFile('plots/L1T_analysis_singleMu5'+file+'.root')
# file12 = TFile('plots/L1T_analysis_singleMu12'+file+'.root')



# ==================   16 GeV ======================
eta16 = file16.Get('heta')
eta_tr16 = file16.Get('heta_trigger')
tg_eta16  = TGraphAsymmErrors(eta_tr16, eta16, '')

pt16 = file16.Get('hpt')
pt_tr16 = file16.Get('hpt_trigger')
tg_pt16  = TGraphAsymmErrors(pt_tr16, pt16, '')


cEta16 = TCanvas('cEta16')
cEta16.cd()
tg_eta16.SetLineColor(kRed)
tg_eta16.SetLineWidth(2)
tg_eta16.SetMarkerStyle(23)
tg_eta16.SetMarkerSize(0.8)
tg_eta16.SetTitle('16 GeV')
tg_eta16.GetXaxis().SetTitle("|#eta(Probe Reco #mu)|")
tg_eta16.GetYaxis().SetTitle("TF Efficiency")
tg_eta16.GetYaxis().SetTitleOffset(1.35)
tg_eta16.GetXaxis().SetNdivisions(509)
tg_eta16.GetYaxis().SetNdivisions(514)
cEta16.SetGridx()
cEta16.SetGridy()
tg_eta16.SetMinimum(0)
tg_eta16.SetMaximum(1.02)
tg_eta16.Draw('AP')



cEta16.SaveAs('plots/png/16_eta_eff'+title+'.pdf')
cEta16.SaveAs('plots/pdf/16_eta_eff'+title+'.png')

# PT
cPt16 = TCanvas('cPt16')
cPt16.cd()
tg_pt16.SetLineColor(kRed)
tg_pt16.SetLineWidth(2)
tg_pt16.SetMarkerStyle(23)
tg_pt16.SetMarkerSize(0.8)
tg_pt16.SetTitle('16 GeV')
tg_pt16.GetXaxis().SetTitle("p_{T}(Probe Reco #mu)")
tg_pt16.GetYaxis().SetTitle("TF Efficiency")
tg_pt16.GetYaxis().SetTitleOffset(1.35)
tg_pt16.GetXaxis().SetNdivisions(509)
tg_pt16.GetYaxis().SetNdivisions(514)
cPt16.SetGridx()
cPt16.SetGridy()
tg_pt16.SetMinimum(0)
tg_pt16.SetMaximum(1.02)
tg_pt16.Draw('AP')

cPt16.SaveAs('plots/pdf/16_pt_eff'+title+'.pdf')
cPt16.SaveAs('plots/png/16_pt_eff'+title+'.png')

# # ====================================================================


# eta0 = file0.Get('heta')
# eta_tr0 = file0.Get('heta_trigger')
# tg_eta0  = TGraphAsymmErrors(eta_tr0, eta0, '')

# pt0 = file0.Get('hpt')
# pt_tr0 = file0.Get('hpt_trigger')
# tg_pt0  = TGraphAsymmErrors(pt_tr0, pt0, '')


# cEta0 = TCanvas('cEta0')
# cEta0.cd()
# tg_eta0.SetLineColor(kRed)
# tg_eta0.SetLineWidth(2)
# tg_eta0.SetMarkerStyle(23)
# tg_eta0.SetMarkerSize(0.8)
# tg_eta0.SetTitle('0 GeV')
# tg_eta0.GetXaxis().SetTitle("|#eta(Probe Reco #mu)|")
# tg_eta0.GetYaxis().SetTitle("TF Efficiency")
# tg_eta0.GetYaxis().SetTitleOffset(1.35)
# tg_eta0.GetXaxis().SetNdivisions(509)
# tg_eta0.GetYaxis().SetNdivisions(514)
# cEta0.SetGridx()
# cEta0.SetGridy()
# tg_eta0.SetMinimum(0)
# tg_eta0.SetMaximum(1.02)
# tg_eta0.Draw('AP')



# cEta0.SaveAs('plots/pdf/0_eta_eff'+title+'.pdf')
# cEta0.SaveAs('plots/png/0_eta_eff'+title+'.png')

# # PT
# cPt0 = TCanvas('cPt0')
# cPt0.cd()
# tg_pt0.SetLineColor(kRed)
# tg_pt0.SetLineWidth(2)
# tg_pt0.SetMarkerStyle(23)
# tg_pt0.SetMarkerSize(0.8)
# tg_pt0.SetTitle('0 GeV')
# tg_pt0.GetXaxis().SetTitle("p_{T}(Probe Reco #mu)")
# tg_pt0.GetYaxis().SetTitle("TF Efficiency")
# tg_pt0.GetYaxis().SetTitleOffset(1.35)
# tg_pt0.GetXaxis().SetNdivisions(509)
# tg_pt0.GetYaxis().SetNdivisions(514)
# cPt0.SetGridx()
# cPt0.SetGridy()
# tg_pt0.SetMinimum(0)
# tg_pt0.SetMaximum(1.02)
# tg_pt0.Draw('AP')

# cPt0.SaveAs('plots/pdf/0_pt_eff'+title+'.pdf')
# cPt0.SaveAs('plots/png/0_pt_eff'+title+'.png')

# # ============================================================

# # ====================================================================


# eta5 = file5.Get('heta')
# eta_tr5 = file5.Get('heta_trigger')
# tg_eta5  = TGraphAsymmErrors(eta_tr5, eta5, '')

# pt5 = file5.Get('hpt')
# pt_tr5 = file5.Get('hpt_trigger')
# tg_pt5  = TGraphAsymmErrors(pt_tr5, pt5, '')


# cEta5 = TCanvas('cEta5')
# cEta5.cd()
# tg_eta5.SetLineColor(kRed)
# tg_eta5.SetLineWidth(2)
# tg_eta5.SetMarkerStyle(23)
# tg_eta5.SetMarkerSize(0.8)
# tg_eta5.SetTitle('5 GeV')
# tg_eta5.GetXaxis().SetTitle("|#eta(Probe Reco #mu)|")
# tg_eta5.GetYaxis().SetTitle("TF Efficiency")
# tg_eta5.GetYaxis().SetTitleOffset(1.35)
# tg_eta5.GetXaxis().SetNdivisions(509)
# tg_eta5.GetYaxis().SetNdivisions(514)
# cEta5.SetGridx()
# cEta5.SetGridy()
# tg_eta5.SetMinimum(0)
# tg_eta5.SetMaximum(1.02)
# tg_eta5.Draw('AP')



# cEta5.SaveAs('plots/pdf/5_eta_eff'+title+'.pdf')
# cEta5.SaveAs('plots/png/5_eta_eff'+title+'.png')

# # ==============================================================

# # ====================================================================


# eta12 = file12.Get('heta')
# eta_tr12 = file12.Get('heta_trigger')
# tg_eta12  = TGraphAsymmErrors(eta_tr12, eta12, '')

# pt12 = file12.Get('hpt')
# pt_tr12 = file12.Get('hpt_trigger')
# tg_pt12  = TGraphAsymmErrors(pt_tr12, pt12, '')


# cEta12 = TCanvas('cEta12')
# cEta12.cd()
# tg_eta12.SetLineColor(kRed)
# tg_eta12.SetLineWidth(2)
# tg_eta12.SetMarkerStyle(23)
# tg_eta12.SetMarkerSize(0.8)
# tg_eta12.SetTitle('12 GeV')
# tg_eta12.GetXaxis().SetTitle("|#eta(Probe Reco #mu)|")
# tg_eta12.GetYaxis().SetTitle("TF Efficiency")
# tg_eta12.GetYaxis().SetTitleOffset(1.35)
# tg_eta12.GetXaxis().SetNdivisions(509)
# tg_eta12.GetYaxis().SetNdivisions(514)
# cEta12.SetGridx()
# cEta12.SetGridy()
# tg_eta12.SetMinimum(0)
# tg_eta12.SetMaximum(1.02)
# tg_eta12.Draw('AP')

# cEta12.SaveAs('plots/pdf/12_eta_eff'+title+'.pdf')
# cEta12.SaveAs('plots/png/12_eta_eff'+title+'.png')


# # ===============================================================

variable_list = [
    ['hpt_emtf', 'EMTF p_{T}', 50, 0, 150],
    ['hpt_bmtf', 'BMTF p_{T}', 50, 0, 150],
    ['hpt_omtf', 'OMTF p_{T}', 50, 0, 150]
    ]

def doPlot(variable, xaxis, nbins, bin_low, bin_high):

    canvas = TCanvas('canvas')
    hist = file16.Get(variable)
    hist.GetXaxis().SetLimits(bin_low, bin_high)
    hist.GetXaxis().SetTitle(xaxis)

    hist.SetFillColor(kYellow)
    hist.SetStats(0)
    hist.Draw()

    #raw_input('Press return to continue...')

    canvas.SaveAs('plots/pdf/'+variable+'.pdf')
    canvas.SaveAs('plots/png/'+variable+'.png')

    canvas.IsA().Destructor(canvas)
    hist.IsA().Destructor(hist)


#for variable in variable_list:
#    doPlot(variable[0], variable[1], variable[2], variable[3], variable[4])


raw_input('Press return to continue...')
