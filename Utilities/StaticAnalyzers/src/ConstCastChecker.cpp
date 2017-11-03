//== ConstCastChecker.cpp - Checks for const_cast<> --------------*- C++ -*--==//
//
// by Thomas Hauth [ Thomas.Hauth@cern.ch ]
//
//===----------------------------------------------------------------------===//


#include <clang/AST/Attr.h>
#include <clang/AST/ExprCXX.h>
#include "ConstCastChecker.h"
#include "CmsSupport.h" 

using namespace clang;
using namespace clang::ento;
using namespace llvm;

namespace clangcms {

void ConstCastChecker::checkPreStmt(const clang::CXXConstCastExpr *CE,
		clang::ento::CheckerContext &C) const
{
	const Expr * SE = CE->getSubExprAsWritten();	
	const CXXRecordDecl * CRD = 0;
	if (SE->getType()->isPointerType()) CRD = SE->getType()->getPointeeCXXRecordDecl();
	else CRD = SE->getType()->getAsCXXRecordDecl();
	if (CRD) {
		std::string cname = CRD->getQualifiedNameAsString();
	}
	if (clang::ento::ExplodedNode *errorNode = C.generateErrorNode()) {
		if (!BT) BT.reset(new clang::ento::BugType(this,"const_cast used on pointer to class", "ConstThreadSafety"));
		std::unique_ptr<clang::ento::BugReport> R = llvm::make_unique<clang::ento::BugReport>(*BT,
					"const_cast was used on a pointer to a class. This may result in thread-unsafe code.", errorNode);
		R->addRange(CE->getSourceRange());
	   	if ( ! m_exception.reportConstCast( *R, C ) )
			return;
		C.emitReport(std::move(R));
	}

}



}


