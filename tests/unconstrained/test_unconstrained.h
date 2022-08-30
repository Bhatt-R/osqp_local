#include "osqp.h"    // OSQP API
#include "osqp_tester.h" // Basic testing script header


#include "unconstrained/data.h"


void test_unconstrained_solve()
{
  OSQPInt exitflag;

  // Problem settings
  OSQPSettings_ptr settings{(OSQPSettings *)c_malloc(sizeof(OSQPSettings))};

  // Structures
  OSQPSolver     *tmpSolver = nullptr;
  OSQPSolver_ptr solver{nullptr};   // Wrap solver inside memory management

  // Populate data
  unconstrained_problem_ptr data{generate_problem_unconstrained()};
  unconstrained_sols_data_ptr sols_data{generate_problem_unconstrained_sols_data()};

  // Define Solver settings as default
  osqp_set_default_settings(settings.get());
  settings->eps_abs = 1e-05;
  settings->eps_rel = 1e-05;
  settings->verbose = 1;

  /* Test all possible linear system solvers in this test case */
  settings->linsys_solver = GENERATE(filter(&isLinsysSupported, values({OSQP_DIRECT_SOLVER, OSQP_INDIRECT_SOLVER})));

  // Setup solver
  exitflag = osqp_setup(&tmpSolver, data->P, data->q,
                        data->A, data->l, data->u,
                        data->m, data->n, settings.get());
  solver.reset(tmpSolver);

  // Setup correct
  mu_assert("Unconstrained test solve: Setup error!", exitflag == 0);

  // Solve Problem first time
  osqp_solve(solver.get());

  // Compare solver statuses
  mu_assert("Unconstrained test solve: Error in solver status!",
            solver->info->status_val == sols_data->status_test);

  // Compare primal solutions
  mu_assert("Unconstrained test solve: Error in primal solution!",
            vec_norm_inf_diff(solver->solution->x, sols_data->x_test,
                              data->n) < TESTS_TOL);

  // Compare objective values
  mu_assert("Unconstrained test solve: Error in objective value!",
            c_absval(solver->info->obj_val - sols_data->obj_value_test) <
            TESTS_TOL);
}
