use crate::server::api;
use crate::server::api::RestResponse as _;
use crate::server::error::ClientError;
use crate::server::result::Result;

async fn post<Req>(client: &reqwest::Client, req: Req) -> Result<reqwest::Response>
where
    Req: api::RestRequest,
{
    // TODO: Make the host/port configurable!
    let url = format!("http://localhost:3088/{}", Req::path());
    client
        .post(url)
        .body(serde_json::to_string(&req)?)
        .send()
        .await
        .map_err(ClientError::ReqwestError)?
        .error_for_status()
        .map_err(ClientError::ReqwestError)
}

pub async fn register(client: &reqwest::Client, req: api::RegisterRequest) -> Result<()> {
    post(client, req).await?;
    Ok(())
}

pub async fn login(client: &reqwest::Client, req: api::LoginRequest) -> Result<api::LoginResponse> {
    let res = post(client, req).await?;
    api::LoginResponse::from_str(&res.text().await?)
}
