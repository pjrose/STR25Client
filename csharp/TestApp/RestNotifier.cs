using System;
using System.Net.Http;
using System.Text;
using System.Text.Json;
using System.Threading;

namespace St25R200.OpenSource;

public sealed class RestNotifier : IDisposable
{
    private readonly HttpClient _client;
    private readonly Uri _baseUri;
    private readonly TimeSpan _timeout;

    public RestNotifier(string baseUri, TimeSpan timeout)
    {
        _baseUri = new Uri(baseUri, UriKind.Absolute);
        _timeout = timeout;
        _client = new HttpClient
        {
            Timeout = timeout,
        };
    }

    public void Dispose() => _client.Dispose();

    public void PostPlaced(string uid, Action<string>? log) => PostUid("placed", uid, log);

    public void PostRemoved(string uid, Action<string>? log) => PostUid("removed", uid, log);

    private void PostUid(string endpoint, string uid, Action<string>? log)
    {
        using var cts = new CancellationTokenSource(_timeout);
        var uri = new Uri(_baseUri, endpoint);
        var payload = JsonSerializer.Serialize(new { uid });
        using var content = new StringContent(payload, Encoding.UTF8, "application/json");
        try
        {
            using var rsp = _client.PostAsync(uri, content, cts.Token).GetAwaiter().GetResult();
            log?.Invoke($"POST {uri} uid={uid} status={(int)rsp.StatusCode}");
        }
        catch (Exception ex) when (ex is OperationCanceledException or HttpRequestException)
        {
            log?.Invoke($"POST {uri} uid={uid} failed: {ex.GetType().Name}");
        }
    }
}
