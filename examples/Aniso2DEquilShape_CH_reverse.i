
# Cahn-Hilliard equation with the reverse split method
# The lower order double-well function was used: sqrt(c^2+e^2)*sqrt((1-c)^2+e^2), which is smoote version of |c|*|1-c|,
# where e is small number. e = 0.01 is recommended.
# Calculates 2D equilibrium shape with an anisotropic interfacial energy
# 4-fold symmetry anisotropic interfacial energy was implemented as an example;
# a(theta) = 1 + eps4 * cos(4*(theta - pi/4))
# The anisotropic kappa and double-well energy function is given by DerivativeParsedMaterial as functions of dc/dx and dc/dy.
# Note that, cos(theta) = (dc/dx)/sqrt((dc/dx)^2 + (dc/dy)^2)
#            sin(theta) = (dc/dy)/sqrt((dc/dx)^2 + (dc/dy)^2).
# How to convert cos(m*theta) into a function of cos(theta) and sin(theta):
# http://mathworld.wolfram.com/Multiple-AngleFormulas.html

[Mesh]
  type = GeneratedMesh
  dim = 2
  nx = 25
  ny = 25
  xmax = 100
  ymax = 100
[]

[Variables]
  [./c]
    order = FIRST
    family = LAGRANGE
  [../]
  [./w]
    order = FIRST
    family = LAGRANGE
  [../]
[]

[AuxVariables]
  [./dcx]
    order = FIRST
    family = MONOMIAL
  [../]
  [./dcy]
    order = FIRST
    family = MONOMIAL
  [../]
  [./time]
  [../]
[]

[AuxKernels]
  [./time]
    type = FunctionAux
    variable = time
    function = t
  [../]
  [./get_dcx]
    type = VariableGradientComponent
    variable = dcx
    gradient_variable = c
    component = x
    execute_on = LINEAR
  [../]
  [./get_dcy]
    type = VariableGradientComponent
    variable = dcy
    gradient_variable = c
    component = y
    execute_on = LINEAR
  [../]
[]

[ICs]
  [./c_circle]
    type = SmoothCircleIC
    variable = c
    x1 = 50
    y1 = 50
    radius = 20
    int_width = 2
    invalue = 1
    outvalue = 0
  [../]
  [./w_zero]
    type = ConstantIC
    variable = w
    value = 0
  [../]
[]

[Kernels]
  #Rc part; chemical potential equation
  [./Rc_mu_itself]
    type = CoupledForce
    variable = c
    v = w
    coef = 1
  [../]
  [./Rc_aniso_doublewell]
    type = AnisotropicDoubleWellEnergy
    variable = c
    mob_name = One
    fbulk_name = f_aniso_m4
    gradient_component_names = 'dcx dcy'
    # fbulk_name = f_iso
  [../]
  [./Rc_aniso_interfacialE]
    type = AnisotropicGradEnergy
    variable = c
    mob_name = One
    kappa_name = kappa_aniso_m4
    gradient_component_names = 'dcx dcy'
    # kappa_name = kappa_iso
  [../]

  #Rmu part; kinetic equation
  [./Rmu_time_derivative_of_c]
    type = CoupledTimeDerivative
    variable = w
    v = c
  [../]

  [./Rmu_div_grad_mu]
    type = SimpleACInterface
    variable = w
    mob_name = One
    kappa_name = M
  [../]
[]

[Materials]
  [./Constants]
    type = GenericConstantMaterial
    # eps4 is the anisotropy strength
    prop_names =  'One M kappa0 w0   eps4'
    prop_values = '1   1 0.2    0.1  0.1'
  [../]

  [./kappa_aniso_m4]
    type = DerivativeParsedMaterial
    f_name = kappa_aniso_m4
    # eps4 is the anisotropy strength
    material_property_names = 'kappa0 eps4'
    constant_names = 'ed'
    constant_expressions = '0.001'
    args = 'dcx dcy'
    #function = 'if(time > 5, kappa0 * aniso_m4, kappa0)'
    function = 'kappa0 * (1 + eps4 * (dcx^4 + dcy^4)/(dcx^2 + dcy^2 + e^2)^2)'
    derivative_order = 2
    outputs = exodus
  [../]

  [./f_aniso_m4]
    type = DerivativeParsedMaterial
    f_name = f_aniso_m4
    # eps4 is the anisotropy strength
    material_property_names = 'w0 eps4'
    constant_names = 'e ed'
    constant_expressions = '0.01 0.001'
    args = 'c dcx dcy'
    function = 'w0 * (sqrt(c^2+e^2)-e)*(sqrt((1-c)^2+e^2)-e) * (1 + eps4 * (dcx^4 + dcy^4)/(dcx^2 + dcy^2 + e^2)^2)'
    derivative_order = 2
    outputs = exodus
  [../]

  [./kappa_iso]
    type = DerivativeParsedMaterial
    f_name = kappa_iso
    material_property_names = 'kappa0'
    function = 'kappa0'
    derivative_order = 2
  [../]

  [./f_iso]
    type = DerivativeParsedMaterial
    f_name = f_iso
    material_property_names = 'w0'
    constant_names = 'e'
    constant_expressions = '0.01'
    args = 'c'
    function = 'w0 * (sqrt(c^2+e^2)-e)*(sqrt((1-c)^2+e^2)-e)'  # smoothed version of w0*|c|*|1-c|
    derivative_order = 2
  [../]

[]

[Preconditioning]
  [./cw_coupling]
    type = SMP
    full = true
  [../]
[]

[Executioner]
  type = Transient
  solve_type = PJFNK
  scheme = bdf2

  # petsc_options_iname = '-pc_type -sub_pc_type'
  # petsc_options_value = 'asm      lu'
  # petsc_options_iname = '-pc_type -pc_asm_overlap'
  # petsc_options_value = 'asm      1'

  petsc_options_iname = '-pc_type -pc_factor_mat_solver_package'
  petsc_options_value = 'lu superlu_dist'

  l_max_its = 20
  l_tol = 1e-4
  nl_max_its = 20
  nl_rel_tol = 1e-8
  nl_abs_tol = 1e-11

  [./TimeStepper]
    type = IterationAdaptiveDT
    dt = 0.005
    growth_factor = 1.2
    cutback_factor = 0.8
    #optimal_iterations = 4
    #iteration_window = 4
  [../]
  # dtmax = 1

  [./Adaptivity]
    initial_adaptivity = 3
    max_h_level = 3
    refine_fraction = 0.95
    coarsen_fraction = 0.10
    weight_names = 'c'
    weight_values = '1.0'
  [../]
[]

[Outputs]
  exodus = true
  print_perf_log = true
[]

[Debug]
 show_var_residual_norms = true
[]
