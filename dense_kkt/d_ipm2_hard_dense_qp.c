/**************************************************************************************************
*                                                                                                 *
* This file is part of HPIPM.                                                                     *
*                                                                                                 *
* HPIPM -- High Performance Interior Point Method.                                                *
* Copyright (C) 2017 by Gianluca Frison.                                                          *
* Developed at IMTEK (University of Freiburg) under the supervision of Moritz Diehl.              *
* All rights reserved.                                                                            *
*                                                                                                 *
* HPMPC is free software; you can redistribute it and/or                                          *
* modify it under the terms of the GNU Lesser General Public                                      *
* License as published by the Free Software Foundation; either                                    *
* version 2.1 of the License, or (at your option) any later version.                              *
*                                                                                                 *
* HPMPC is distributed in the hope that it will be useful,                                        *
* but WITHOUT ANY WARRANTY; without even the implied warranty of                                  *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                                            *
* See the GNU Lesser General Public License for more details.                                     *
*                                                                                                 *
* You should have received a copy of the GNU Lesser General Public                                *
* License along with HPMPC; if not, write to the Free Software                                    *
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA                  *
*                                                                                                 *
* Author: Gianluca Frison, gianluca.frison (at) imtek.uni-freiburg.de                             *
*                                                                                                 *
**************************************************************************************************/



#include <blasfeo_target.h>
#include <blasfeo_common.h>
#include <blasfeo_d_aux.h>

#include "../include/hpipm_d_dense_qp.h"
#include "../include/hpipm_d_ipm2_hard_dense_qp.h"
#include "../include/hpipm_d_ipm2_hard_revcom_qp.h"



int d_memsize_ipm2_hard_dense_qp(struct d_dense_qp *qp, struct d_ipm2_hard_dense_qp_arg *arg)
	{

	int nv = qp->nv;
	int ne = qp->ne;
	int nb = qp->nb;
	int ng = qp->ng;

	int size = 0;

	size += 33*sizeof(struct d_strvec); // v pi lam lam_lb lam_ub lam_lg lam_ug t t_lb t_ub t_lg t_ug dv dpi dlam dt dt_lb dt_ub dt_lg dt_ug res_g res_b res_d res_d_lb res_d_ub res_d_lg res_d_ug res_m Qx qx Dv lv tmp_nb
	size += 3*sizeof(struct d_strmat); // Lv AL Le

	size += d_size_strvec(nb); // tmp_nb
	size += d_size_strvec(nv); // lv
	size += d_size_strmat(nv, nv); // Lv
	size += d_size_strmat(ne, nv); // AL
	size += d_size_strmat(ne, ne); // Le

	size += 1*sizeof(struct d_ipm2_hard_revcom_qp_workspace);
	size += d_memsize_ipm2_hard_revcom_qp(qp->nv, qp->ne, qp->nb, qp->ng, arg->iter_max);

	size += 1^64; // update to typical cache line size

	return size;

	}



void d_create_ipm2_hard_dense_qp(struct d_dense_qp *qp, struct d_ipm2_hard_dense_qp_arg *arg, struct d_ipm2_hard_dense_qp_workspace *workspace, void *mem)
	{

	int nv = qp->nv;
	int ne = qp->ne;
	int nb = qp->nb;
	int ng = qp->ng;


	// revcom struct
	struct d_ipm2_hard_revcom_qp_workspace *sr_ptr = mem;

	// revcom workspace
	workspace->revcom_workspace = sr_ptr;
	sr_ptr += 1;
	struct d_ipm2_hard_revcom_qp_workspace *rwork = workspace->revcom_workspace;


	// matrix struct
	struct d_strmat *sm_ptr = (struct d_strmat *) sr_ptr;

	workspace->Lv = sm_ptr;
	sm_ptr += 1;
	workspace->AL = sm_ptr;
	sm_ptr += 1;
	workspace->Le = sm_ptr;
	sm_ptr += 1;


	// vector struct
	struct d_strvec *sv_ptr = (struct d_strvec *) sm_ptr;

	workspace->v = sv_ptr;
	sv_ptr += 1;
	workspace->pi = sv_ptr;
	sv_ptr += 1;
	workspace->lam = sv_ptr;
	sv_ptr += 1;
	workspace->lam_lb = sv_ptr;
	sv_ptr += 1;
	workspace->lam_ub = sv_ptr;
	sv_ptr += 1;
	workspace->lam_lg = sv_ptr;
	sv_ptr += 1;
	workspace->lam_ug = sv_ptr;
	sv_ptr += 1;
	workspace->t = sv_ptr;
	sv_ptr += 1;
	workspace->t_lb = sv_ptr;
	sv_ptr += 1;
	workspace->t_ub = sv_ptr;
	sv_ptr += 1;
	workspace->t_lg = sv_ptr;
	sv_ptr += 1;
	workspace->t_ug = sv_ptr;
	sv_ptr += 1;
	workspace->dv = sv_ptr;
	sv_ptr += 1;
	workspace->dpi = sv_ptr;
	sv_ptr += 1;
	workspace->dlam = sv_ptr;
	sv_ptr += 1;
	workspace->dt = sv_ptr;
	sv_ptr += 1;
	workspace->dt_lb = sv_ptr;
	sv_ptr += 1;
	workspace->dt_ub = sv_ptr;
	sv_ptr += 1;
	workspace->dt_lg = sv_ptr;
	sv_ptr += 1;
	workspace->dt_ug = sv_ptr;
	sv_ptr += 1;
	workspace->res_g = sv_ptr;
	sv_ptr += 1;
	workspace->res_b = sv_ptr;
	sv_ptr += 1;
	workspace->res_d = sv_ptr;
	sv_ptr += 1;
	workspace->res_d_lb = sv_ptr;
	sv_ptr += 1;
	workspace->res_d_ub = sv_ptr;
	sv_ptr += 1;
	workspace->res_d_lg = sv_ptr;
	sv_ptr += 1;
	workspace->res_d_ug = sv_ptr;
	sv_ptr += 1;
	workspace->res_m = sv_ptr;
	sv_ptr += 1;
	workspace->Qx = sv_ptr;
	sv_ptr += 1;
	workspace->qx = sv_ptr;
	sv_ptr += 1;
	workspace->Dv = sv_ptr;
	sv_ptr += 1;
	workspace->lv = sv_ptr;
	sv_ptr += 1;
	workspace->tmp_nb = sv_ptr;
	sv_ptr += 1;


	// align to typicl cache line size
	long long l_ptr = (long long) sv_ptr;
	l_ptr = (l_ptr+63)/64*64;


	// void stuf
	void *v_ptr = (void *) l_ptr;

	d_create_strmat(nv, nv, workspace->Lv, v_ptr);
	v_ptr += workspace->Lv->memory_size;

	d_create_strmat(ne, nv, workspace->AL, v_ptr);
	v_ptr += workspace->AL->memory_size;

	d_create_strmat(ne, ne, workspace->Le, v_ptr);
	v_ptr += workspace->Le->memory_size;

	d_create_strvec(nv, workspace->lv, v_ptr);
	v_ptr += workspace->lv->memory_size;

	d_create_strvec(nb, workspace->tmp_nb, v_ptr);
	v_ptr += workspace->tmp_nb->memory_size;

	rwork->nv = nv;
	rwork->ne = ne;
	rwork->nb = nb;
	rwork->ng = ng;
	rwork->iter_max = arg->iter_max;
	d_create_ipm2_hard_revcom_qp(rwork, v_ptr);
	v_ptr += workspace->revcom_workspace->memsize;

	rwork->alpha_min = arg->alpha_min;
	rwork->mu_max = arg->mu_max;
	rwork->mu0 = arg->mu0;


	// alias members of workspace and revcom_workspace
	d_create_strvec(nv, workspace->v, rwork->v);
	d_create_strvec(ne, workspace->pi, rwork->pi);
	d_create_strvec(2*nb+2*ng, workspace->lam, rwork->lam);
	d_create_strvec(nb, workspace->lam_lb, rwork->lam_lb);
	d_create_strvec(nb, workspace->lam_ub, rwork->lam_ub);
	d_create_strvec(ng, workspace->lam_lg, rwork->lam_lg);
	d_create_strvec(ng, workspace->lam_ug, rwork->lam_ug);
	d_create_strvec(2*nb+2*ng, workspace->t, rwork->t);
	d_create_strvec(nb, workspace->t_lb, rwork->t_lb);
	d_create_strvec(nb, workspace->t_ub, rwork->t_ub);
	d_create_strvec(ng, workspace->t_lg, rwork->t_lg);
	d_create_strvec(ng, workspace->t_ug, rwork->t_ug);
	d_create_strvec(nv, workspace->dv, rwork->dv);
	d_create_strvec(ne, workspace->dpi, rwork->dpi);
	d_create_strvec(2*nb+2*ng, workspace->dlam, rwork->dlam);
	d_create_strvec(2*nb+2*ng, workspace->dt, rwork->dt);
	d_create_strvec(nb, workspace->dt_lb, rwork->dt_lb);
	d_create_strvec(nb, workspace->dt_ub, rwork->dt_ub);
	d_create_strvec(ng, workspace->dt_lg, rwork->dt_lg);
	d_create_strvec(ng, workspace->dt_ug, rwork->dt_ug);
	d_create_strvec(nv, workspace->res_g, rwork->res_g);
	d_create_strvec(ne, workspace->res_b, rwork->res_b);
	d_create_strvec(2*nb+2*ng, workspace->res_d, rwork->res_d);
	d_create_strvec(nb, workspace->res_d_lb, rwork->res_d_lb);
	d_create_strvec(nb, workspace->res_d_ub, rwork->res_d_ub);
	d_create_strvec(ng, workspace->res_d_lg, rwork->res_d_lg);
	d_create_strvec(ng, workspace->res_d_ug, rwork->res_d_ug);
	d_create_strvec(2*nb+2*ng, workspace->res_m, rwork->res_m);
	d_create_strvec(nb+ng, workspace->Qx, rwork->Qx);
	d_create_strvec(nb+ng, workspace->qx, rwork->qx);
	d_create_strvec(ng, workspace->Dv, rwork->Dv);


	workspace->nt_inv = 1.0/(2*nb+2*ng);

	return;

	}



void d_solve_ipm2_hard_dense_qp(struct d_dense_qp *qp, struct d_ipm2_hard_dense_qp_workspace *ws)
	{

	struct d_ipm2_hard_revcom_qp_workspace *rws = ws->revcom_workspace;

	int nv = qp->nv;
	int ne = qp->ne;
	int nb = qp->nb;
	int ng = qp->ng;

	// alias qp vectors into revcom workspace
	rws->d = qp->d->pa;
	rws->d_lb = qp->d_lb->pa;
	rws->d_ub = qp->d_ub->pa;
	rws->d_lg = qp->d_lg->pa;
	rws->d_ug = qp->d_ug->pa;

	// init solver
	rws->entry = INIT_RES;
	d_ipm2_hard_revcom_qp(rws);

#if 0
	// XXX hard-code solution for debug
	ws->v->pa[0] = 0.25;
	ws->v->pa[1] = 0.75;
	ws->pi->pa[0] = 2.75;
	ws->lam_lb->pa[0] = 0.0;
	ws->lam_lb->pa[1] = 0.0;
	ws->lam_ub->pa[0] = 0.0;
	ws->lam_ub->pa[1] = 0.0;
	ws->t_lb->pa[0] = 1.25;
	ws->t_lb->pa[1] = 1.75;
	ws->t_ub->pa[0] = 1.75;
	ws->t_ub->pa[1] = 1.25;
#endif

	// compute residuals
	d_compute_res_dense_qp(qp, ws);
	rws->mu = ws->res_mu;

#if 0
	printf("\nres_g\n");
	d_print_tran_strvec(nv, ws->res_g, 0);
	printf("\nres_b\n");
	d_print_tran_strvec(ne, ws->res_b, 0);
	printf("\nres_d\n");
	d_print_tran_strvec(2*nb+2*ng, ws->res_d, 0);
	printf("\nres_m\n");
	d_print_tran_strvec(2*nb+2*ng, ws->res_m, 0);
#endif

	int kk;
	for(kk=0; kk<10; kk++)
		{

//		printf("\niter %d\n", kk);

		// fact and solve kkt
		d_fact_solve_kkt_step_dense_qp(qp, ws);

		// alpha
		d_compute_alpha_dense_qp(rws);

		//
		d_update_var_dense_qp(rws);

#if 0
		printf("\nv\n");
		d_print_tran_strvec(nv, ws->v, 0);
		printf("\npi\n");
		d_print_tran_strvec(ne, ws->pi, 0);
		printf("\nlam\n");
		d_print_tran_strvec(2*nb+2*ng, ws->lam, 0);
		printf("\nt\n");
		d_print_tran_strvec(2*nb+2*ng, ws->t, 0);
#endif

		// compute residuals
		d_compute_res_dense_qp(qp, ws);
		rws->mu = ws->res_mu;
#if 0

		printf("\nres_g\n");
		d_print_e_tran_strvec(nv, ws->res_g, 0);
		printf("\nres_b\n");
		d_print_e_tran_strvec(ne, ws->res_b, 0);
		printf("\nres_d\n");
		d_print_e_tran_strvec(2*nb+2*ng, ws->res_d, 0);
		printf("\nres_m\n");
		d_print_e_tran_strvec(2*nb+2*ng, ws->res_m, 0);
#endif

		}

	return;

	}
