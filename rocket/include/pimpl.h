#ifndef _PIMPL_H
#define _PIMPL_H



#define PIMPL_DEF  private: struct PYLON_API impl;  boost::shared_ptr< impl>  _pimpl;
#define IMPL_METHOD(RETURN,METHOD)  RETURN CLS::METHOD() { return _pimpl->METHOD(); }
#define IMPL_METHOD1(RETURN,METHOD,T1)  RETURN CLS::METHOD(T1 v1) { return _pimpl->METHOD(v1); }
#define IMPL_METHOD2(RETURN,METHOD,T1,T2)  RETURN CLS::METHOD(T1 v1,T2 v2) { return _pimpl->METHOD(v1,v2); }
#endif
