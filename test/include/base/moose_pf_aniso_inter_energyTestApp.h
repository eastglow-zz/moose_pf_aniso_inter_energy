//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html
#ifndef MOOSE_PF_ANISO_INTER_ENERGYTESTAPP_H
#define MOOSE_PF_ANISO_INTER_ENERGYTESTAPP_H

#include "MooseApp.h"

class moose_pf_aniso_inter_energyTestApp;

template <>
InputParameters validParams<moose_pf_aniso_inter_energyTestApp>();

class moose_pf_aniso_inter_energyTestApp : public MooseApp
{
public:
  moose_pf_aniso_inter_energyTestApp(InputParameters parameters);
  virtual ~moose_pf_aniso_inter_energyTestApp();

  static void registerApps();
  static void registerObjects(Factory & factory);
  static void associateSyntax(Syntax & syntax, ActionFactory & action_factory);
  static void registerExecFlags(Factory & factory);
};

#endif /* MOOSE_PF_ANISO_INTER_ENERGYTESTAPP_H */
