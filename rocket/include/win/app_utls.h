#pragma  once
namespace yyou
{

    /** 程序单实例
     * @brief 
     */
	class  app_sigeton
	{
		HANDLE _mutex;
		bool		_have_run;
	public:
		app_sigeton(const wchar_t *name);
        /** 
         * @brief 确认是否已有进程实例运行
         * 
         * @return 
         */
		bool have_instance();
		~app_sigeton();
	};

}
