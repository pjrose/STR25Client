using System;
using St25R200.OpenSource;

namespace St25R200.OpenSource.Sample;

internal static class Program
{
    private static void Main(string[] args)
    {
        string port = args.Length > 0 ? args[0] : "COM30";
        var options = new St25r200ReaderService.Options(
            PortName: port,
            RestBaseUri: "http://192.168.1.100/",
            RestTimeoutMs: 200,
            MaxTrackedTags: 4,
            Debug: St25r200ReaderService.DebugLevel.Frames
        );

        using var svc = new St25r200ReaderService(options);
        svc.Log += Console.WriteLine;
        svc.TagArrived += uid => Console.WriteLine($"ARRIVED {uid}");
        svc.TagLeft += uid => Console.WriteLine($"LEFT {uid}");

        svc.Start();
        Console.WriteLine("Press ENTER to stop...");
        Console.ReadLine();
        svc.Stop();
    }
}
