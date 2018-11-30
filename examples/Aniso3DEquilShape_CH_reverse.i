
# Cahn-Hilliard equation with the reverse split method
# The lower order double-well function was used: sqrt(c^2+e^2)*sqrt((1-c)^2+e^2), which is smoote version of |c|*|1-c|,
# where e is small number. e = 0.01 is recommended.
# Calculates 3D equilibrium shape with an anisotropic interfacial energy
# 4-fold symmetry anisotropic interfacial energy was implemented as an example;
# ref: Eq. (97) of Phys. Rev. E, 57 (1998) 4323

[Mesh]
  type = GeneratedMesh
  dim = 3
  nx = 10
  ny = 10
  nz = 10
  xmax = 100
  ymax = 100
  zmax = 100
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
  [./dcz]
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
  [./get_dcz]
    type = VariableGradientComponent
    variable = dcz
    gradient_variable = c
    component = z
    execute_on = LINEAR
  [../]
[]

[ICs]
  [./c_circle]
    type = SmoothCircleIC
    variable = c
    x1 = 0
    y1 = 0
    z1 = 0
    radius = 40
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
    gradient_component_names = 'dcx dcy dcz'
    # fbulk_name = f_iso
  [../]
  [./Rc_aniso_interfacialE]
    type = AnisotropicGradEnergy
    variable = c
    mob_name = One
    kappa_name = kappa_aniso_m4
    gradient_component_names = 'dcx dcy dcz'
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
    prop_values = '1   1 4      1    0.4'
  [../]

  [./kappa_aniso_m4]
    type = DerivativeParsedMaterial
    f_name = kappa_aniso_m4
    # eps4 is the anisotropy strength
    material_property_names = 'kappa0 eps4'
    constant_names = 'e'
    constant_expressions = '0.01'
    args = 'dcx dcy dcz'
    function = 'kappa0 * (1 + eps4 * (dcx^4 + dcy^4 + dcz^4)/(dcx^2 + dcy^2 + dcz^2 + e^2)^2)'
    derivative_order = 2
    outputs = exodus
  [../]

  [./f_aniso_m4]
    type = DerivativeParsedMaterial
    f_name = f_aniso_m4
    # eps4 is the anisotropy strength
    material_property_names = 'w0 eps4'
    constant_names = 'e'
    constant_expressions = '0.01'
    args = 'c dcx dcy dcz'
    function = 'w0 * (sqrt(c^2+e^2)-e)*(sqrt((1-c)^2+e^2)-e) * (1 + eps4 * (dcx^4 + dcy^4 + dcz^4)/(dcx^2 + dcy^2 + dcz^2 + e^2)^2)'
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
  end_time = 1e6

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
