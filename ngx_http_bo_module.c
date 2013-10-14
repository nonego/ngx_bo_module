#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>
#include "BOModuleInterface.h"

/* Module config */
static char *conf_file=NULL;

static char *ngx_http_bo_conf(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);

/* Directives */
static ngx_command_t  ngx_http_bo_commands[] = {
    { ngx_string("bo_conf"),
      NGX_HTTP_LOC_CONF|NGX_CONF_TAKE1,
      ngx_http_bo_conf,
      0,
      0,
      NULL },
      ngx_null_command
};
/* Http context of the module */
static ngx_http_module_t  ngx_http_bo_module_ctx = {
    NULL,          /* preconfiguration */
    NULL,          /* postconfiguration */
    NULL,          /* create main configuration */
    NULL,          /* init main configuration */
    NULL,          /* create server configuration */
    NULL,          /* merge server configuration */
    NULL,          /* create location configration */
    NULL           /* merge location configration */
};

static ngx_int_t ngx_http_bo_init(ngx_cycle_t *cycle)
{
	return ( 0 == BOInit( conf_file ) ? NGX_OK : NGX_ERROR );
}

static void ngx_http_bo_destroy(ngx_cycle_t *cycle)
{
	BODestroy();
	if(conf_file)
	{
		free(conf_file);
		conf_file=NULL;
	}
}

/* Module */
ngx_module_t  ngx_http_bo_module = {
    NGX_MODULE_V1,
    &ngx_http_bo_module_ctx,             /* module context */
    ngx_http_bo_commands,                /* module directives */
    NGX_HTTP_MODULE,                       /* module type */
    NULL,                                  /* init master */
    NULL,                                  /* init module */
    ngx_http_bo_init,                                  /* init process */
    NULL,                                  /* init thread */
    NULL,                                  /* exit thread */
    NULL,                                  /* exit process */
    ngx_http_bo_destroy,                                  /* exit master */
    NGX_MODULE_V1_PADDING
};

/* Handler function */
static ngx_int_t
ngx_http_bo_handler(ngx_http_request_t *r)
{
    ngx_int_t rc;
    ngx_buf_t *b;
    ngx_chain_t out;
		char *content=NULL;
		int content_len=0;
		 
		if(!(r->method & (NGX_HTTP_HEAD|NGX_HTTP_GET|NGX_HTTP_POST)))
    {
        return NGX_HTTP_NOT_ALLOWED;
    }
    
		r->headers_out.content_type.len = g_content_type.len;
    r->headers_out.content_type.data = (u_char *) g_content_type.str;

    r->headers_out.status = NGX_HTTP_OK;
    r->headers_out.content_length_n = 0;

    if(r->method == NGX_HTTP_HEAD)
    {
        rc = ngx_http_send_header(r);
        if(rc != NGX_OK)
        {
            return rc;
        }
    }

		rc = BOProcess( (const char*)r->uri.data, r->uri.len, (const char*)r->args.data, r->args.len, &content, &content_len );
		if( rc != 0 || content == NULL)
			return NGX_HTTP_INTERNAL_SERVER_ERROR;

    b = ngx_pcalloc(r->pool, sizeof(ngx_buf_t));
    if(b == NULL)
    {
        ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "Failed to allocate response buffer.");
        return NGX_HTTP_INTERNAL_SERVER_ERROR;
    }
		b->pos = (u_char *)ngx_palloc(r->pool, content_len);
    if(b->pos == NULL)
    {
        ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "Failed to allocate buffer for response content.");
        return NGX_HTTP_INTERNAL_SERVER_ERROR;
    }
		b->last = b->pos + content_len;
		memcpy(b->pos, (void *)content, content_len);
		BOFree(&content);
    b->memory = 1;
    b->last_buf = 1;
    out.buf = b;
    out.next = NULL;

		r->headers_out.content_length_n = content_len;
    rc = ngx_http_send_header(r);
    if(rc != NGX_OK)
      return rc;

    return ngx_http_output_filter(r, &out);
}

static char *
ngx_http_bo_conf(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)
{
    ngx_http_core_loc_conf_t  *clcf;
    clcf = ngx_http_conf_get_module_loc_conf(cf, ngx_http_core_module);
    clcf->handler = ngx_http_bo_handler;

		ngx_str_t *conf_args = cf->args->elts;
		++conf_args;

		conf_file = malloc(conf_args->len + 1);
		memcpy(conf_file, conf_args->data, conf_args->len);
		conf_file[conf_args->len] = 0;

		return NGX_CONF_OK;
}
