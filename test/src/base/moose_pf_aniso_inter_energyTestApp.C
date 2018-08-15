//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html
#include "moose_pf_aniso_inter_energyTestApp.h"
#include "moose_pf_aniso_inter_energyApp.h"
#include "Moose.h"
#include "AppFactory.h"
#include "MooseSyntax.h"
#include "ModulesApp.h"

template <>
InputParameters
validParams<moose_pf_aniso_inter_energyTestApp>()
{
  InputParameters params = validParams<moose_pf_aniso_inter_energyApp>();
  return params;
}

moose_pf_aniso_inter_energyTestApp::moose_pf_aniso_inter_energyTestApp(InputParameters parameters) : MooseApp(parameters)
{
  Moose::registerObjects(_factory);
  ModulesApp::registerObjects(_factory);
  moose_pf_aniso_inter_energyApp::registerObjectDepends(_factory);
  moose_pf_aniso_inter_energyApp::registerObjects(_factory);

  Moose::associateSyntax(_syntax, _action_factory);
  ModulesApp::associateSyntax(_syntax, _action_factory);
  moose_pf_aniso_inter_energyApp::associateSyntaxDepends(_syntax, _action_factory);
  moose_pf_aniso_inter_energyApp::associateSyntax(_syntax, _action_factory);

  Moose::registerExecFlags(_factory);
  ModulesApp::registerExecFlags(_factory);
  moose_pf_aniso_inter_energyApp::registerExecFlags(_factory);

  bool use_test_objs = getParam<bool>("allow_test_objects");
  if (use_test_objs)
  {
    moose_pf_aniso_inter_energyTestApp::registerObjects(_factory);
    moose_pf_aniso_inter_energyTestApp::associateSyntax(_syntax, _action_factory);
    moose_pf_aniso_inter_energyTestApp::registerExecFlags(_factory);
  }
}

moose_pf_aniso_inter_energyTestApp::~moose_pf_aniso_inter_energyTestApp() {}

void
moose_pf_aniso_inter_energyTestApp::registerApps()
{
  registerApp(moose_pf_aniso_inter_energyApp);
  registerApp(moose_pf_aniso_inter_energyTestApp);
}

void
moose_pf_aniso_inter_energyTestApp::registerObjects(Factory & /*factory*/)
{
  /* Uncomment Factory parameter and register your new test objects here! */
}

void
moose_pf_aniso_inter_energyTestApp::associateSyntax(Syntax & /*syntax*/, ActionFactory & /*action_factory*/)
{
  /* Uncomment Syntax and ActionFactory parameters and register your new test objects here! */
}

void
moose_pf_aniso_inter_energyTestApp::registerExecFlags(Factory & /*factory*/)
{
  /* Uncomment Factory parameter and register your new execute flags here! */
}

/***************************************************************************************************
 *********************** Dynamic Library Entry Points - DO NOT MODIFY ******************************
 **************************************************************************************************/
// External entry point for dynamic application loading
extern "C" void
moose_pf_aniso_inter_energyTestApp__registerApps()
{
  moose_pf_aniso_inter_energyTestApp::registerApps();
}

// External entry point for dynamic object registration
extern "C" void
moose_pf_aniso_inter_energyTestApp__registerObjects(Factory & factory)
{
  moose_pf_aniso_inter_energyTestApp::registerObjects(factory);
}

// External entry point for dynamic syntax association
extern "C" void
moose_pf_aniso_inter_energyTestApp__associateSyntax(Syntax & syntax, ActionFactory & action_factory)
{
  moose_pf_aniso_inter_energyTestApp::associateSyntax(syntax, action_factory);
}

// External entry point for dynamic execute flag loading
extern "C" void
moose_pf_aniso_inter_energyTestApp__registerExecFlags(Factory & factory)
{
  moose_pf_aniso_inter_energyTestApp::registerExecFlags(factory);
}
