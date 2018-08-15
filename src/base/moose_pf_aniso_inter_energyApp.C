#include "moose_pf_aniso_inter_energyApp.h"
#include "Moose.h"
#include "AppFactory.h"
#include "ModulesApp.h"
#include "MooseSyntax.h"

template <>
InputParameters
validParams<moose_pf_aniso_inter_energyApp>()
{
  InputParameters params = validParams<MooseApp>();
  return params;
}

moose_pf_aniso_inter_energyApp::moose_pf_aniso_inter_energyApp(InputParameters parameters) : MooseApp(parameters)
{
  Moose::registerObjects(_factory);
  ModulesApp::registerObjects(_factory);
  moose_pf_aniso_inter_energyApp::registerObjects(_factory);

  Moose::associateSyntax(_syntax, _action_factory);
  ModulesApp::associateSyntax(_syntax, _action_factory);
  moose_pf_aniso_inter_energyApp::associateSyntax(_syntax, _action_factory);

  Moose::registerExecFlags(_factory);
  ModulesApp::registerExecFlags(_factory);
  moose_pf_aniso_inter_energyApp::registerExecFlags(_factory);
}

moose_pf_aniso_inter_energyApp::~moose_pf_aniso_inter_energyApp() {}

void
moose_pf_aniso_inter_energyApp::registerApps()
{
  registerApp(moose_pf_aniso_inter_energyApp);
}

void
moose_pf_aniso_inter_energyApp::registerObjects(Factory & factory)
{
    Registry::registerObjectsTo(factory, {"moose_pf_aniso_inter_energyApp"});
}

void
moose_pf_aniso_inter_energyApp::associateSyntax(Syntax & /*syntax*/, ActionFactory & action_factory)
{
  Registry::registerActionsTo(action_factory, {"moose_pf_aniso_inter_energyApp"});

  /* Uncomment Syntax parameter and register your new production objects here! */
}

void
moose_pf_aniso_inter_energyApp::registerObjectDepends(Factory & /*factory*/)
{
}

void
moose_pf_aniso_inter_energyApp::associateSyntaxDepends(Syntax & /*syntax*/, ActionFactory & /*action_factory*/)
{
}

void
moose_pf_aniso_inter_energyApp::registerExecFlags(Factory & /*factory*/)
{
  /* Uncomment Factory parameter and register your new execution flags here! */
}

/***************************************************************************************************
 *********************** Dynamic Library Entry Points - DO NOT MODIFY ******************************
 **************************************************************************************************/
extern "C" void
moose_pf_aniso_inter_energyApp__registerApps()
{
  moose_pf_aniso_inter_energyApp::registerApps();
}

extern "C" void
moose_pf_aniso_inter_energyApp__registerObjects(Factory & factory)
{
  moose_pf_aniso_inter_energyApp::registerObjects(factory);
}

extern "C" void
moose_pf_aniso_inter_energyApp__associateSyntax(Syntax & syntax, ActionFactory & action_factory)
{
  moose_pf_aniso_inter_energyApp::associateSyntax(syntax, action_factory);
}

extern "C" void
moose_pf_aniso_inter_energyApp__registerExecFlags(Factory & factory)
{
  moose_pf_aniso_inter_energyApp::registerExecFlags(factory);
}
