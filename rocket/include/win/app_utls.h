#pragma  once
namespace yyou
{

    /** ����ʵ��
     * @brief 
     */
	class  app_sigeton
	{
		HANDLE _mutex;
		bool		_have_run;
	public:
		app_sigeton(const wchar_t *name);
        /** 
         * @brief ȷ���Ƿ����н���ʵ������
         * 
         * @return 
         */
		bool have_instance();
		~app_sigeton();
	};

}
