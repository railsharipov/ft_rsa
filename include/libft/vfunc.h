/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   libft.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rsharipo <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/07/09 13:36:51 by rsharipo          #+#    #+#             */
/*   Updated: 2018/10/13 09:00:28 by rsharipo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIBFT_VFUNC_H
# define LIBFT_VFUNC_H

/* Overload macros on number of args (up to 10 args) */
# define __NARG__(...)	__NARG_I_(__VA_ARGS__ __VA_OPT__(,) 10,9,8,7,6,5,4,3,2,1,0)
# define __NARG_I_(...)	__ARG_N(__VA_ARGS__)
# define __ARG_N(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,N,...) N

# define _VFUNC_(name, n)	name##n
# define _VFUNC(name, n)	_VFUNC_(name, n)
# define VFUNC(func, ...)	_VFUNC(func, __NARG__(__VA_ARGS__)) (__VA_ARGS__)

#endif