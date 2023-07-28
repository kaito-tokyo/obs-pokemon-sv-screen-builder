#include <functional>

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Web.Http.Headers.h>
#include <winrt/windows.storage.streams.h>

using winrt::Windows::Foundation::Uri;
using winrt::Windows::Storage::Streams::IBuffer;
using winrt::Windows::Web::Http::HttpClient;
using winrt::Windows::Web::Http::HttpResponseMessage;
using winrt::Windows::Web::Http::IHttpContent;

void fetchStringFromUrl(const char *urlString,
			std::function<void(std::string, int)> callback)
{
	winrt::init_apartment();

	HttpClient httpClient;
	auto headers(httpClient.DefaultRequestHeaders());
	Uri requestUri(winrt::to_hstring(urlString));
	HttpResponseMessage httpResponseMessage;
	IBuffer httpResponseBuffer;
	httpResponseMessage = httpClient.GetAsync(requestUri).get();
	httpResponseMessage.EnsureSuccessStatusCode();
	IHttpContent httpContent = httpResponseMessage.Content();
	httpResponseBuffer = httpContent.ReadAsBufferAsync().get();

	uint8_t *data = httpResponseBuffer.data();
	std::string str((const char *)data, httpResponseBuffer.Length());
	callback(str, 0);
}