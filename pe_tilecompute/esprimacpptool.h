/// 用于将脚本解析成AST，然后进行相关操作
/// 引入头文件后，在cpp文件定义宏 #define ESPRIMA_CPP_TOOL_IMPLEMENTATION
///

//sample codes:
/*
#define ESPRIMA_CPP_TOOL_IMPLEMENTATION
#include "esprimacpptool.h"
int main(int argc, char *argv[]) {
    std::string codeText = "let a=pe.Dataset(dsnameVar,1); \
                            pe.Dataset('fy4',2); \
                            const f = function(){ \
                              var c = pe.Dataset('ls/08',4) ; \
                            }; \
                            function main(){ \
                                let ds=pe.Dataset('mod/ndvi',5); \
                                return ds; \
                            } \
                           " ;
    std::vector<std::string> dsNameArr ;
    std::string errorMsg ;
    int64_t dura_ms = 0;
    bool isok = EsprimaCppTool::getDatasetNameArray(codeText , dsNameArr ,errorMsg, dura_ms);
    std::cout<<"isok:"<<isok<<std::endl ;
    std::cout<<"message:"<<errorMsg<<std::endl ;
    std::cout<<"dura_ms:"<<dura_ms<<std::endl ;
    for(int i = 0 ; i<dsNameArr.size();++i ){
      std::cout<<i<<" - "<< dsNameArr[i]<<std::endl ;
    }
    return 0;
}
*/


//2022-2-12 wf
//v1.0
//(1) 目前只检索数据集名称 包括：pe.Dataset('name',...) pe.DatasetArray('name',...) pe.Datafile('name',...)
//(2) 上面数据集名称必须是字符串常量，不能是变量。
//(3) 该工具类只检查整个脚本在语法层面的合法性和数据集名称，不对脚本算法的有效性检查

//2022-7-26
//v1.1
//(1) add pe.DatasetCollection , pe.DatasetCollections for datasetname.


#ifndef ESPRIMA_CPP_TOOL_H
#define ESPRIMA_CPP_TOOL_H

#include "esprima.h"
#include <sys/time.h>
#include <iostream>
#include <vector>
#include <string>
using namespace esprima ;

//#include <sys/time.h>
//#include <string>
//#include "esprima.h"
//#include <vector>
//using namespace esprima ;


struct PeDatasetCallerCondition {
  bool isCallExpression ;
  bool hasMemberExpression ;
  bool hasMemberExpressionIdentifierPe ;
  int  typeOfMemberExpressionIdentifierDatasetName ;//0-none,1-Dataset,2-DatasetArray,3-DataFile,4-datasetcollection,5-datasetcollections
  bool hasArgumentsStringLiteral ;
  std::string dsName;
  inline PeDatasetCallerCondition():isCallExpression(false),hasMemberExpression(false),hasMemberExpressionIdentifierPe(false),typeOfMemberExpressionIdentifierDatasetName(0),hasArgumentsStringLiteral(false){}
  inline bool isGood(){if(isCallExpression&&hasMemberExpression&&hasMemberExpressionIdentifierPe&&typeOfMemberExpressionIdentifierDatasetName>0&&hasArgumentsStringLiteral&&dsName.length()>0)return true;return false;}
} ;

struct PeDatasetVisitor : esprima::Visitor {
  std::vector<std::string> dsnameArray ;
  inline PeDatasetVisitor(){nodeStack.reserve(10);dsnameArray.reserve(10);}

  std::vector<PeDatasetCallerCondition> nodeStack ;

  virtual void visit(Program *node) ;
  virtual void visit(Identifier *node) ;
  virtual void visit(BlockStatement *node) ;
  virtual void visit(EmptyStatement *node) ;
  virtual void visit(ExpressionStatement *node) ;
  virtual void visit(IfStatement *node) ;
  virtual void visit(LabeledStatement *node) ;
  virtual void visit(BreakStatement *node) ;
  virtual void visit(ContinueStatement *node) ;
  virtual void visit(WithStatement *node) ;
  virtual void visit(SwitchCase *node) ;
  virtual void visit(SwitchStatement *node) ;
  virtual void visit(ReturnStatement *node) ;
  virtual void visit(ThrowStatement *node) ;
  virtual void visit(CatchClause *node) ;
  virtual void visit(TryStatement *node) ;
  virtual void visit(WhileStatement *node) ;
  virtual void visit(DoWhileStatement *node) ;
  virtual void visit(ForStatement *node) ;
  virtual void visit(ForInStatement *node) ;
  virtual void visit(DebuggerStatement *node) ;
  virtual void visit(FunctionDeclaration *node) ;
  virtual void visit(VariableDeclarator *node) ;
  virtual void visit(VariableDeclaration *node) ;
  virtual void visit(ThisExpression *node) ;
  virtual void visit(ArrayExpression *node) ;
  virtual void visit(Property *node) ;
  virtual void visit(ObjectExpression *node) ;
  virtual void visit(FunctionExpression *node) ;
  virtual void visit(SequenceExpression *node) ;
  virtual void visit(UnaryExpression *node) ;
  virtual void visit(BinaryExpression *node) ;
  virtual void visit(AssignmentExpression *node) ;
  virtual void visit(UpdateExpression *node) ;
  virtual void visit(LogicalExpression *node) ;
  virtual void visit(ConditionalExpression *node) ;
  virtual void visit(NewExpression *node) ;
  virtual void visit(CallExpression *node) ;
  virtual void visit(MemberExpression *node) ;
  virtual void visit(NullLiteral *node) ;
  virtual void visit(RegExpLiteral *node) ;
  virtual void visit(StringLiteral *node) ;
  virtual void visit(NumericLiteral *node) ;
  virtual void visit(BooleanLiteral *node) ;
};

class EsprimaCppTool {
   public:
        static bool getDatasetNameArray(std::string codeText,
                                        std::vector<std::string>& dsNameArr,
                                        std::string& error,
                                         int64_t& dura_ms);
        static int64_t microseconds() ;
} ;

#endif

/////////////////////////////////////////////// IMPLEMENTATION ////////
#ifdef ESPRIMA_CPP_TOOL_IMPLEMENTATION

int64_t EsprimaCppTool::microseconds() {
  struct timeval time;
  gettimeofday(&time, NULL);
  return time.tv_sec * (int64_t)1000000 + time.tv_usec;
}
bool EsprimaCppTool::getDatasetNameArray(std::string codeText,
                                         std::vector<std::string>& dsNameArr,
                                         std::string& message,
                                         int64_t& dura_ms )
{
    dura_ms = 0 ;
    try {
      esprima::Pool pool;
      int64_t start = microseconds();
      esprima::Program *program = esprima::parse(pool, codeText);
      int64_t end = microseconds();

      PeDatasetVisitor visitor;
      program->accept(&visitor);
      dura_ms = (end - start) / 1000 ;
      dsNameArr = visitor.dsnameArray ;
      return true ;
    } catch (const esprima::ParseError &error) {
      message = std::string("parse error:")+error.description;
      return false ;
    }
}


void PeDatasetVisitor::visit(Program *node) {nodeStack.push_back(PeDatasetCallerCondition());visitChildren(node);nodeStack.pop_back();}
void PeDatasetVisitor::visit(BlockStatement *node) {nodeStack.push_back(PeDatasetCallerCondition());visitChildren(node);nodeStack.pop_back(); }
void PeDatasetVisitor::visit(EmptyStatement *node) {nodeStack.push_back(PeDatasetCallerCondition());visitChildren(node);nodeStack.pop_back(); }
void PeDatasetVisitor::visit(ExpressionStatement *node) {nodeStack.push_back(PeDatasetCallerCondition());visitChildren(node);nodeStack.pop_back(); }
void PeDatasetVisitor::visit(IfStatement *node) {nodeStack.push_back(PeDatasetCallerCondition());visitChildren(node);nodeStack.pop_back(); }
void PeDatasetVisitor::visit(LabeledStatement *node) {nodeStack.push_back(PeDatasetCallerCondition());visitChildren(node);nodeStack.pop_back(); }
void PeDatasetVisitor::visit(BreakStatement *node) {nodeStack.push_back(PeDatasetCallerCondition());visitChildren(node);nodeStack.pop_back(); }
void PeDatasetVisitor::visit(ContinueStatement *node) {nodeStack.push_back(PeDatasetCallerCondition());visitChildren(node);nodeStack.pop_back(); }
void PeDatasetVisitor::visit(WithStatement *node) {nodeStack.push_back(PeDatasetCallerCondition());visitChildren(node);nodeStack.pop_back(); }
void PeDatasetVisitor::visit(SwitchCase *node) {nodeStack.push_back(PeDatasetCallerCondition());visitChildren(node);nodeStack.pop_back(); }
void PeDatasetVisitor::visit(SwitchStatement *node) {nodeStack.push_back(PeDatasetCallerCondition());visitChildren(node);nodeStack.pop_back(); }
void PeDatasetVisitor::visit(ReturnStatement *node) {nodeStack.push_back(PeDatasetCallerCondition());visitChildren(node);nodeStack.pop_back(); }
void PeDatasetVisitor::visit(ThrowStatement *node) {nodeStack.push_back(PeDatasetCallerCondition());visitChildren(node);nodeStack.pop_back(); }
void PeDatasetVisitor::visit(CatchClause *node) {nodeStack.push_back(PeDatasetCallerCondition());visitChildren(node);nodeStack.pop_back(); }
void PeDatasetVisitor::visit(TryStatement *node) {nodeStack.push_back(PeDatasetCallerCondition());visitChildren(node);nodeStack.pop_back(); }
void PeDatasetVisitor::visit(WhileStatement *node) {nodeStack.push_back(PeDatasetCallerCondition());visitChildren(node);nodeStack.pop_back(); }
void PeDatasetVisitor::visit(DoWhileStatement *node) {nodeStack.push_back(PeDatasetCallerCondition());visitChildren(node);nodeStack.pop_back(); }
void PeDatasetVisitor::visit(ForStatement *node) {nodeStack.push_back(PeDatasetCallerCondition());visitChildren(node);nodeStack.pop_back(); }
void PeDatasetVisitor::visit(ForInStatement *node) {nodeStack.push_back(PeDatasetCallerCondition());visitChildren(node);nodeStack.pop_back(); }
void PeDatasetVisitor::visit(DebuggerStatement *node) {nodeStack.push_back(PeDatasetCallerCondition());visitChildren(node);nodeStack.pop_back(); }
void PeDatasetVisitor::visit(FunctionDeclaration *node) {nodeStack.push_back(PeDatasetCallerCondition());visitChildren(node);nodeStack.pop_back(); }
void PeDatasetVisitor::visit(VariableDeclarator *node) {nodeStack.push_back(PeDatasetCallerCondition());visitChildren(node);nodeStack.pop_back(); }
void PeDatasetVisitor::visit(VariableDeclaration *node) {nodeStack.push_back(PeDatasetCallerCondition());visitChildren(node);nodeStack.pop_back(); }
void PeDatasetVisitor::visit(ThisExpression *node) {nodeStack.push_back(PeDatasetCallerCondition());visitChildren(node);nodeStack.pop_back(); }
void PeDatasetVisitor::visit(ArrayExpression *node) {nodeStack.push_back(PeDatasetCallerCondition());visitChildren(node);nodeStack.pop_back(); }
void PeDatasetVisitor::visit(Property *node) {nodeStack.push_back(PeDatasetCallerCondition());visitChildren(node);nodeStack.pop_back(); }
void PeDatasetVisitor::visit(ObjectExpression *node) {nodeStack.push_back(PeDatasetCallerCondition());visitChildren(node);nodeStack.pop_back(); }
void PeDatasetVisitor::visit(FunctionExpression *node) {nodeStack.push_back(PeDatasetCallerCondition());visitChildren(node);nodeStack.pop_back(); }
void PeDatasetVisitor::visit(SequenceExpression *node) {nodeStack.push_back(PeDatasetCallerCondition());visitChildren(node);nodeStack.pop_back(); }
void PeDatasetVisitor::visit(UnaryExpression *node) {nodeStack.push_back(PeDatasetCallerCondition());visitChildren(node);nodeStack.pop_back(); }
void PeDatasetVisitor::visit(BinaryExpression *node) {nodeStack.push_back(PeDatasetCallerCondition());visitChildren(node);nodeStack.pop_back(); }
void PeDatasetVisitor::visit(AssignmentExpression *node) {nodeStack.push_back(PeDatasetCallerCondition());visitChildren(node);nodeStack.pop_back(); }
void PeDatasetVisitor::visit(UpdateExpression *node) {nodeStack.push_back(PeDatasetCallerCondition());visitChildren(node);nodeStack.pop_back(); }
void PeDatasetVisitor::visit(LogicalExpression *node) {nodeStack.push_back(PeDatasetCallerCondition());visitChildren(node);nodeStack.pop_back(); }
void PeDatasetVisitor::visit(ConditionalExpression *node) {nodeStack.push_back(PeDatasetCallerCondition());visitChildren(node);nodeStack.pop_back(); }
void PeDatasetVisitor::visit(NewExpression *node) {nodeStack.push_back(PeDatasetCallerCondition());visitChildren(node);nodeStack.pop_back(); }

void PeDatasetVisitor::visit(Identifier *node) {
  //name: PixelEngine, pe, Dataset, DatasetArray, DataFile DatasetCollection DatasetCollections
  nodeStack.push_back(PeDatasetCallerCondition());
  if( nodeStack.size() >=3 ){
      if( nodeStack[nodeStack.size()-3].isCallExpression==true
          && nodeStack[nodeStack.size()-3].hasMemberExpression==true )
      {
        if( node->name.compare("pe")==0 || node->name.compare("PixelEngine")==0 ){
          nodeStack[nodeStack.size()-3].hasMemberExpressionIdentifierPe=true ;
        }else if( node->name.compare("Dataset")==0 ){
          nodeStack[nodeStack.size()-3].typeOfMemberExpressionIdentifierDatasetName=1;
        }else if( node->name.compare("DatasetArray")==0 ){
          nodeStack[nodeStack.size()-3].typeOfMemberExpressionIdentifierDatasetName=2;
        }else if( node->name.compare("DataFile")==0 ){
          nodeStack[nodeStack.size()-3].typeOfMemberExpressionIdentifierDatasetName=3;
        }else if( node->name.compare("DatasetCollection")==0 ){
          nodeStack[nodeStack.size()-3].typeOfMemberExpressionIdentifierDatasetName=4;
        }else if( node->name.compare("DatasetCollections")==0 ){
          nodeStack[nodeStack.size()-3].typeOfMemberExpressionIdentifierDatasetName=5;
        }
      }
  }
  visitChildren(node);
  nodeStack.pop_back();
}

void PeDatasetVisitor::visit(CallExpression *node) {
  //
  PeDatasetCallerCondition c;
  c.isCallExpression = true ;
  nodeStack.push_back(c);
  visitChildren(node);
  if( nodeStack.back().isGood()==true ){
    dsnameArray.push_back(nodeStack.back().dsName) ;
  }
  nodeStack.pop_back();
}

void PeDatasetVisitor::visit(MemberExpression *node) {
  //
  nodeStack.push_back(PeDatasetCallerCondition());
  if( nodeStack.size() >= 2 ){
    if( nodeStack[nodeStack.size()-2].isCallExpression==true ){
      nodeStack[nodeStack.size()-2].hasMemberExpression=true;
    }
  }
  visitChildren(node);
  nodeStack.pop_back();
}
void PeDatasetVisitor::visit(StringLiteral *node) {
  //
  nodeStack.push_back(PeDatasetCallerCondition());
  if( nodeStack.size() >= 2 ){
    if( nodeStack[nodeStack.size()-2].isCallExpression==true ){
      nodeStack[nodeStack.size()-2].hasArgumentsStringLiteral=true;
      nodeStack[nodeStack.size()-2].dsName = node->value ;
    }
  }
  visitChildren(node);
  nodeStack.pop_back();
}

void PeDatasetVisitor::visit(NullLiteral *node) {nodeStack.push_back(PeDatasetCallerCondition());visitChildren(node);nodeStack.pop_back(); }
void PeDatasetVisitor::visit(RegExpLiteral *node) {nodeStack.push_back(PeDatasetCallerCondition());visitChildren(node);nodeStack.pop_back(); }
void PeDatasetVisitor::visit(NumericLiteral *node) {nodeStack.push_back(PeDatasetCallerCondition());visitChildren(node);nodeStack.pop_back(); }
void PeDatasetVisitor::visit(BooleanLiteral *node) {nodeStack.push_back(PeDatasetCallerCondition());visitChildren(node);nodeStack.pop_back(); }

#endif


