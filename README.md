oled_ui_module
===

oled_ui_module software framework
create by chenty


������д��һ��ui֧�ֿ�

Э����Ҫ���� ���� 5110 ���ֵ�ɫ 8λ�� lcd �� OLED����ʾ����

ui�����ƱȽϼ� ԭ���ǰ�һ����Ļ���ֳ�һ��һ���Ŀؼ� Ȼ����ز�ͬ�Ŀؼ�ʵ����ʾ�Լ���ͬ�Ĺ���

ȥ���˺��ϲ��Լ��ײ�Ĺ��� ģ�黯�������� �Ա���ֲ��ʹ��

Ŀǰʵ�ֵĿؼ���3��

str	�ı��ؼ�
img 	ͼƬ�ؼ�
btn	��ť�ؼ�

���е���ʾ�ؼ���������ͬ�Ļ��� view
struct ui_view_param_t
{
	struct ui_view_layout_t layout;
	uint32_t	flags;
};

�ؼ�ʵ�����·���
struct ui_view_op_t
{
	vsf_err_t 			(*init)(void *param);
	vsf_err_t 			(*fini)(void *param);
	struct ui_view_layout_t* 	(*onlayout)(void *param);
	vsf_err_t 			(*ondraw)(void *param, struct ui_view_buf_t *viewbuf);
	vsf_err_t			(*onfocus)(void *param, uint8_t isfocused);
	vsf_err_t 			(*onevt)(void *param, uint8_t evt);
};

ui��ͨ�����ò�ͬ�ķ�����ʵ�ֿؼ�����ʾ �۽� ���


ui���ʹ�÷�������

������Ҫ�ṩ�����ò���

��ʾ���򳤶�
#define DISP_SIZE_H		128
��ʾ�������� ����oled ���� 5510���ֵ�ɫ��Ļ
#define DISP_SIZE_V		8
�ؼ��������
#define UI_CELL_MAX		20

������ 
�˵�����ֵ
#define KEY_VALUE_MENU			KEY_VALUE_KEY0
ȷ������ֵ
#define KEY_VALUE_ENTER			KEY_VALUE_KEY1




������Ҫ���� uiʹ�õĿռ� һ�㵥Ƭ������ �ͳ�ʼ����̬�ṹ�弴��
�� struct ui_param_t ui;

����uiģ���ṩ���õķ���
struct ui_op_t
{
	vsf_err_t 	(*init)(struct ui_param_t *param);
	vsf_err_t 	(*fini)(struct ui_param_t *param);
	vsf_err_t 	(*ondisp)(struct ui_param_t *param, struct vsf_buffer_t *dpbuf);
	vsf_err_t 	(*onkey)(struct ui_param_t *param, uint8_t key);	
	vsf_err_t	(*addview)(struct ui_param_t *param, void *view, const struct ui_view_op_t *type);
	vsf_err_t	(*delview)(struct ui_param_t *param, void *view);
};


������Ҫ��uiģ���ʼ��
ui_op.init(&ui);

Ȼ��ui ģ��Ϳ���ʹ����
��ʱû���κοؼ� ������Ҫ ע��ؼ���uiģ��
�ؼ�����Ϊ���� Ҳ����Ϊ���� Ϊ�˽�ʡ�ڴ� �������ʾ���ݿ��Դ洢��flash����

���� �ı��ؼ�
const struct ui_viewstr_param_t bootstring = 
{
	{0, 6, 128, 2}, UI_VIEW_FLAG_ENABLE,
	"���ڿ���" ,UI_VIEW_STRBIG
};

�ؼ���ע����Ҫ�ṩ2������ һ���ǿؼ�����Ľṹ�� һ���ǿؼ��ķ��� ����ط�Ӧ���иĽ��Ŀؼ� �����������������
ui_op.addview(&ui, (void *)&bootstring, &ui_viewstr_op);

��ʱ�ؼ���ע�����
�����ʾ������

��Ҫ���²��� 
������Ҫ�ṩһ��������ʾ������ڴ�
ͨ��struct vsf_buffer_t �ṹ�崫��
��ʾ�ڴ�ռ���Ҫ��װ 0 1 2 3 ��˳���Ų� ��Ŀǰ��

ÿ�ε��� ui_op.ondisp(&ui, &dpbuf); ��ˢ��һ����ʾ ��btn �ؼ��� ���ˢ���ٶȾ���focus��˸���ٶ�

�����÷��� �ڶ�ʱ����ѭ������ ͬʱ�ڸ�������ʱ ֱ�ӵ���


���ְ����¼��� ��Ҫ������evt ���ݸ�uiģ�� ui���Զ������л�focus ���� ���ÿؼ� click�¼��Ȳ���

������ ui_op.onkey(&ui, keyvalue);


������ر� �����л�ʱ ��2�ַ�ʽ һ�ֿ��԰ѿؼ�DISABLE һ����del�ؼ�

del�ؼ�������������
ui_op.delview(&ui, (void *)&bootstring);

������Ҫui�ͷ��ڴ�ʱ���� 
ui_op.fini(&ui);

��̬������ڴ�ui�����ͷ���Ҫ����ϵͳ����������Ӧ�ĺ����ͷ��ڴ�

Todo:�ؼ�ʹ�����





