#ifndef QEXP_H
#define QEXP_H

#include <stdio.h>
#include <stdlib.h>
#include <qstring.h>
#include <qvaluestack.h>
#include <iostream>
#include <klocale.h>

#include "analitza.h"

/**
	@author Aleix Pol i Gonzalez
*/

typedef enum {
	S,
	R,
	A,
	E1,
	E2,
	E3,
	E,
	K
} actEnum;

typedef enum {
	tAssig, 	//:=
	tLambda,	//->
	tLimits,	//..
	tAdd,		//+
	tSub,		//-
	tMul,		//*
	tDiv,		// /
	tPow,		//^
	tUmi,		//-a
	tFunc,		//f(x)
	tComa,		//,
	tLpr,		//(
	tRpr,		//)
	tEof,
	tMaxOp,
	tVal,
	tVar
} tokEnum;

struct TOKEN {
	QString val;
	tokEnum tipus;
};

// void printPilaOpr(QValueStack<int> opr);
// QString opr2str(int);

